#include "store.h"

#include <sqlite3.h>
#include <iostream>

// ---- small helpers -----------------------------------------------------

static void bindBlob(sqlite3_stmt *st, int idx, const Bytes &b) {
	sqlite3_bind_blob(st, idx, b.data(), (int)b.size(), SQLITE_TRANSIENT);
}

static Bytes columnBlob(sqlite3_stmt *st, int idx) {
	const unsigned char *p = (const unsigned char *)sqlite3_column_blob(st, idx);
	int n = sqlite3_column_bytes(st, idx);
	return Bytes(p, p + n);
}

// ---- lifecycle ---------------------------------------------------------

Store::Store(const std::string &path) {
	if (sqlite3_open(path.c_str(), &db) != SQLITE_OK)
		std::cerr << "Cannot open DB: " << sqlite3_errmsg(db) << "\n";
}

Store::~Store() {
	if (db)
		sqlite3_close(db);
}

void Store::exec(const std::string &sql) {
	char *err = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
		std::cerr << "sqlite error: " << (err ? err : "") << "\n";
		sqlite3_free(err);
	}
}

void Store::initSchema() {
	exec("CREATE TABLE IF NOT EXISTS identity ("
	     "  id INTEGER PRIMARY KEY, ed_priv BLOB, ed_pub BLOB, x_priv BLOB, x_pub BLOB);");
	exec("CREATE TABLE IF NOT EXISTS signed_prekey ("
	     "  id INTEGER PRIMARY KEY, priv BLOB, pub BLOB, sig BLOB, created_at INTEGER);");
	exec("CREATE TABLE IF NOT EXISTS one_time_prekeys ("
	     "  key_id INTEGER PRIMARY KEY, priv BLOB, pub BLOB, used INTEGER DEFAULT 0);");
	// Reserved for later milestones (the ratchet slot, contacts pinned via TOFU, inbox):
	exec("CREATE TABLE IF NOT EXISTS sessions ("
	     "  peer TEXT PRIMARY KEY, sk BLOB, send_ctr INTEGER DEFAULT 0, recv_ctr INTEGER DEFAULT 0);");
	exec("CREATE TABLE IF NOT EXISTS contacts (peer TEXT PRIMARY KEY, ed_pub BLOB);");
	exec("CREATE TABLE IF NOT EXISTS inbox ("
	     "  id INTEGER PRIMARY KEY AUTOINCREMENT, peer TEXT, plaintext TEXT, ts INTEGER);");
}

// ---- identity ----------------------------------------------------------

bool Store::hasIdentity() {
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM identity WHERE id=1;", -1, &st, nullptr);
	bool exists = (sqlite3_step(st) == SQLITE_ROW) && (sqlite3_column_int(st, 0) > 0);
	sqlite3_finalize(st);
	return exists;
}

void Store::saveIdentity(const Identity &id) {
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db,
	    "INSERT OR REPLACE INTO identity(id, ed_priv, ed_pub, x_priv, x_pub) VALUES(1,?,?,?,?);",
	    -1, &st, nullptr);
	bindBlob(st, 1, id.edPriv);
	bindBlob(st, 2, id.edPub);
	bindBlob(st, 3, id.xPriv);
	bindBlob(st, 4, id.xPub);
	sqlite3_step(st);
	sqlite3_finalize(st);
}

Identity Store::loadIdentity() {
	Identity id;
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db, "SELECT ed_priv, ed_pub, x_priv, x_pub FROM identity WHERE id=1;", -1, &st, nullptr);
	if (sqlite3_step(st) == SQLITE_ROW) {
		id.edPriv = columnBlob(st, 0);
		id.edPub = columnBlob(st, 1);
		id.xPriv = columnBlob(st, 2);
		id.xPub = columnBlob(st, 3);
	}
	sqlite3_finalize(st);
	return id;
}

// ---- signed prekey -----------------------------------------------------

void Store::saveSignedPreKey(const SignedPreKey &spk) {
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db,
	    "INSERT OR REPLACE INTO signed_prekey(id, priv, pub, sig, created_at) VALUES(1,?,?,?,?);",
	    -1, &st, nullptr);
	bindBlob(st, 1, spk.priv);
	bindBlob(st, 2, spk.pub);
	bindBlob(st, 3, spk.sig);
	sqlite3_bind_int64(st, 4, spk.createdAt);
	sqlite3_step(st);
	sqlite3_finalize(st);
}

SignedPreKey Store::loadSignedPreKey() {
	SignedPreKey spk;
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db, "SELECT priv, pub, sig, created_at FROM signed_prekey WHERE id=1;", -1, &st, nullptr);
	if (sqlite3_step(st) == SQLITE_ROW) {
		spk.priv = columnBlob(st, 0);
		spk.pub = columnBlob(st, 1);
		spk.sig = columnBlob(st, 2);
		spk.createdAt = (long)sqlite3_column_int64(st, 3);
	}
	sqlite3_finalize(st);
	return spk;
}

// ---- one-time prekeys --------------------------------------------------

void Store::addOneTimePreKey(const OneTimePreKey &opk) {
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db,
	    "INSERT OR REPLACE INTO one_time_prekeys(key_id, priv, pub, used) VALUES(?,?,?,0);",
	    -1, &st, nullptr);
	sqlite3_bind_int(st, 1, opk.id);
	bindBlob(st, 2, opk.priv);
	bindBlob(st, 3, opk.pub);
	sqlite3_step(st);
	sqlite3_finalize(st);
}

std::vector<OneTimePreKey> Store::loadOneTimePreKeys() {
	std::vector<OneTimePreKey> keys;
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db, "SELECT key_id, priv, pub FROM one_time_prekeys ORDER BY key_id;", -1, &st, nullptr);
	while (sqlite3_step(st) == SQLITE_ROW) {
		OneTimePreKey opk;
		opk.id = sqlite3_column_int(st, 0);
		opk.priv = columnBlob(st, 1);
		opk.pub = columnBlob(st, 2);
		keys.push_back(opk);
	}
	sqlite3_finalize(st);
	return keys;
}

OneTimePreKey Store::loadOneTimePreKey(int id) {
	OneTimePreKey opk;
	opk.id = id;
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db, "SELECT priv, pub FROM one_time_prekeys WHERE key_id=?;", -1, &st, nullptr);
	sqlite3_bind_int(st, 1, id);
	if (sqlite3_step(st) == SQLITE_ROW) {
		opk.priv = columnBlob(st, 0);
		opk.pub = columnBlob(st, 1);
	}
	sqlite3_finalize(st);
	return opk;
}

void Store::markOneTimePreKeyUsed(int id) {
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db, "UPDATE one_time_prekeys SET used=1 WHERE key_id=?;", -1, &st, nullptr);
	sqlite3_bind_int(st, 1, id);
	sqlite3_step(st);
	sqlite3_finalize(st);
}

// ---- sessions ----------------------------------------------------------

void Store::saveSession(const std::string &peer, const Bytes &sk) {
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db,
	    "INSERT OR REPLACE INTO sessions(peer, sk, send_ctr, recv_ctr) VALUES(?,?,0,0);",
	    -1, &st, nullptr);
	sqlite3_bind_text(st, 1, peer.c_str(), -1, SQLITE_TRANSIENT);
	bindBlob(st, 2, sk);
	sqlite3_step(st);
	sqlite3_finalize(st);
}

Bytes Store::getSessionKey(const std::string &peer) {
	Bytes sk;
	sqlite3_stmt *st = nullptr;
	sqlite3_prepare_v2(db, "SELECT sk FROM sessions WHERE peer=?;", -1, &st, nullptr);
	sqlite3_bind_text(st, 1, peer.c_str(), -1, SQLITE_TRANSIENT);
	if (sqlite3_step(st) == SQLITE_ROW)
		sk = columnBlob(st, 0);
	sqlite3_finalize(st);
	return sk;
}
