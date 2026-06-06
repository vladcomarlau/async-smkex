#ifndef ACLIENT_STORE_H
#define ACLIENT_STORE_H

#include <string>
#include <vector>
#include "aclient_crypto.h"

struct sqlite3; // forward declaration (defined in <sqlite3.h>)

// This device's long-term identity: an Ed25519 signing key + an X25519 DH key.
struct Identity {
	Bytes edPriv, edPub;   // Ed25519 (signs attestations / signed PreKey)
	Bytes xPriv, xPub;     // X25519  (DH1, DH2)
};

// The medium-term signed PreKey and its Ed25519 signature.
struct SignedPreKey {
	Bytes priv, pub, sig;
	long  createdAt = 0;
};

// A single-use one-time PreKey, identified by a small id (Signal-style).
struct OneTimePreKey {
	int   id = 0;
	Bytes priv, pub;
};

/*
 * Local persistent state for the client (the "local database with session
 * state and keys"). Backed by SQLite. The server never sees any of this;
 * private keys live here so an offline recipient can still decrypt later.
 */
class Store {
public:
	explicit Store(const std::string &path);
	~Store();

	void initSchema();

	// identity
	bool     hasIdentity();
	void     saveIdentity(const Identity &id);
	Identity loadIdentity();

	// signed prekey
	void         saveSignedPreKey(const SignedPreKey &spk);
	SignedPreKey loadSignedPreKey();

	// one-time prekeys
	void                       addOneTimePreKey(const OneTimePreKey &opk);
	std::vector<OneTimePreKey> loadOneTimePreKeys();
	OneTimePreKey              loadOneTimePreKey(int id);   // empty priv if not found
	void                       markOneTimePreKeyUsed(int id);

	// sessions (the "ratchet slot": for now just the derived secret SK)
	void  saveSession(const std::string &peer, const Bytes &sk);
	Bytes getSessionKey(const std::string &peer);

private:
	sqlite3 *db = nullptr;
	void exec(const std::string &sql);
};

#endif
