#include "aclient_crypto.h"
#include "store.h"
#include "http.h"

#include <ctime>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <future>

// Number of one-time PreKeys generated at registration.
static const int OPK_COUNT = 5;

// Bundles older than this are considered stale (freshness window, seconds).
static const long FRESHNESS_WINDOW = 7 * 24 * 3600; //7 days

// Default server locations (overridable on the command line).
static const std::string DEFAULT_PREKEY = "http://localhost:8081";
static const std::string DEFAULT_NOTARY = "http://localhost:8082";

// ---- canonical serialization (shared by Bob and Alice) -----------------
//
// The long-term "core" of the bundle is the exact byte string that both the
// signer (Bob) and the verifier (Alice) must agree on. Fixed-size fields in a
// fixed order, so the hash is reproducible on both sides.

static void append(Bytes &dst, const Bytes &src) {
	dst.insert(dst.end(), src.begin(), src.end());
}

static Bytes u64be(uint64_t v) {
	Bytes b(8);
	for (int i = 7; i >= 0; i--) {
		b[i] = (unsigned char)(v & 0xff);
		v >>= 8;
	}
	return b;
}

static Bytes serializeCore(const Bytes &edPub, const Bytes &xPub, const Bytes &spkPub,
                           const Bytes &spkSig, uint64_t timestamp) {
	Bytes core;
	append(core, edPub);
	append(core, xPub);
	append(core, spkPub);
	append(core, spkSig);
	append(core, u64be(timestamp));
	return core;
}

// Digest that the attestation signs: SHA-256(core || OPK).
static Bytes attestationDigest(const Bytes &core, const Bytes &opkPub) {
	Bytes msg = core;
	append(msg, opkPub);
	return acrypto::sha256(msg);
}

// ---- minimal JSON readers (server JSON is flat; values are base64/numbers) ----

// extracts value as string for specific key from json
static std::string jsonStr(const std::string &body, const std::string &key) {
	std::string pat = "\"" + key + "\"";
	size_t k = body.find(pat);
	if (k == std::string::npos)
		return "";
	size_t colon = body.find(':', k + pat.size());
	if (colon == std::string::npos)
		return "";
	size_t q1 = body.find('"', colon);
	if (q1 == std::string::npos)
		return "";
	size_t q2 = body.find('"', q1 + 1);
	if (q2 == std::string::npos)
		return "";
	return body.substr(q1 + 1, q2 - q1 - 1);
}

// extracts value as long number for specific key from json
static long jsonLong(const std::string &body, const std::string &key) {
	std::string pat = "\"" + key + "\"";
	size_t k = body.find(pat);
	if (k == std::string::npos)
		return -1;
	size_t colon = body.find(':', k + pat.size());
	if (colon == std::string::npos)
		return -1;
	size_t i = colon + 1;
	while (i < body.size() && body[i] == ' ')
		i++;
	long val = 0;
	bool any = false;
	while (i < body.size() && body[i] >= '0' && body[i] <= '9') {
		val = val * 10 + (body[i] - '0');
		i++;
		any = true;
	}
	return any ? val : -1;
}

// extracts all quoted strings from a flat JSON array of strings, e.g. ["a","b"]
static std::vector<std::string> jsonStringArray(const std::string &body) {
	std::vector<std::string> out;
	size_t i = 0;
	while (true) {
		size_t q1 = body.find('"', i);
		if (q1 == std::string::npos)
			break;
		size_t q2 = body.find('"', q1 + 1);
		if (q2 == std::string::npos)
			break;
		out.push_back(body.substr(q1 + 1, q2 - q1 - 1));
		i = q2 + 1;
	}
	return out;
}

// ---- commands: init / show / selftest ----------------------------------

static void doInit(Store &store) {
	if (store.hasIdentity()) {
		std::cout << "Already initialized. Use 'show' to inspect.\n";
		return;
	}

	Identity id;
	acrypto::ed25519_keypair(id.edPriv, id.edPub);
	acrypto::x25519_keypair(id.xPriv, id.xPub);
	store.saveIdentity(id);

	SignedPreKey spk;
	acrypto::x25519_keypair(spk.priv, spk.pub);
	spk.sig = acrypto::ed25519_sign(id.edPriv, spk.pub);
	spk.createdAt = (long)time(nullptr);
	store.saveSignedPreKey(spk);

	for (int i = 0; i < OPK_COUNT; i++) {
		OneTimePreKey opk;
		opk.id = i;
		acrypto::x25519_keypair(opk.priv, opk.pub);
		store.addOneTimePreKey(opk);
	}

	std::cout << "Initialized identity and prekeys.\n";
	std::cout << "  Identity (Ed25519): " << acrypto::b64encode(id.edPub) << "\n";
	std::cout << "  Identity (X25519):  " << acrypto::b64encode(id.xPub) << "\n";
	std::cout << "  Signed PreKey:      " << acrypto::b64encode(spk.pub) << "\n";
	std::cout << "  One-time PreKeys:   " << OPK_COUNT << " (ids 0.." << (OPK_COUNT - 1) << ")\n";
}

static void doShow(Store &store) {
	if (!store.hasIdentity()) {
		std::cout << "Not initialized yet. Run 'init' first.\n";
		return;
	}

	Identity id = store.loadIdentity();
	SignedPreKey spk = store.loadSignedPreKey();
	std::vector<OneTimePreKey> opks = store.loadOneTimePreKeys();

	std::cout << "Identity (Ed25519): " << acrypto::b64encode(id.edPub) << "\n";
	std::cout << "Identity (X25519):  " << acrypto::b64encode(id.xPub) << "\n";
	std::cout << "Signed PreKey:      " << acrypto::b64encode(spk.pub) << "\n";
	std::cout << "Signed PreKey sig:  " << (acrypto::ed25519_verify(id.edPub, spk.pub, spk.sig) ? "VALID" : "INVALID") << "\n";
	std::cout << "One-time PreKeys (" << opks.size() << "):\n";
	for (const auto &opk : opks)
		std::cout << "  [" << opk.id << "] " << acrypto::b64encode(opk.pub) << "\n";
}

static void doSelfTest() {
	std::cout << "== crypto self-test ==\n";
	bool allOk = true;

	Bytes edPriv, edPub;
	acrypto::ed25519_keypair(edPriv, edPub);
	Bytes msg = {'h', 'e', 'l', 'l', 'o'};
	Bytes sig = acrypto::ed25519_sign(edPriv, msg);
	bool good = acrypto::ed25519_verify(edPub, msg, sig);
	Bytes tampered = msg;
	tampered[0] = 'H';
	bool rejectsTamper = !acrypto::ed25519_verify(edPub, tampered, sig);
	std::cout << "  Ed25519 sign/verify:       " << (good ? "ok" : "FAIL")
	          << " | rejects tamper: " << (rejectsTamper ? "ok" : "FAIL") << "\n";
	allOk &= good && rejectsTamper;

	Bytes aPriv, aPub, bPriv, bPub;
	acrypto::x25519_keypair(aPriv, aPub);
	acrypto::x25519_keypair(bPriv, bPub);
	Bytes ssA = acrypto::x25519_dh(aPriv, bPub);
	Bytes ssB = acrypto::x25519_dh(bPriv, aPub);
	bool dhAgrees = (!ssA.empty() && ssA == ssB);
	std::cout << "  X25519 DH agreement:       " << (dhAgrees ? "ok" : "FAIL") << "\n";
	allOk &= dhAgrees;

	Bytes xPriv, xPub, spkPriv, spkPub, opkPriv, opkPub;
	acrypto::x25519_keypair(xPriv, xPub);
	acrypto::x25519_keypair(spkPriv, spkPub);
	acrypto::x25519_keypair(opkPriv, opkPub);
	Bytes spkSig = acrypto::ed25519_sign(edPriv, spkPub);
	Bytes core = serializeCore(edPub, xPub, spkPub, spkSig, 1730000000ULL);
	Bytes digest = attestationDigest(core, opkPub);
	Bytes attestation = acrypto::ed25519_sign(edPriv, digest);
	bool attOk = acrypto::ed25519_verify(edPub, digest, attestation);
	std::cout << "  Attestation build/verify:  " << (attOk ? "ok" : "FAIL") << "\n";
	allOk &= attOk;

	std::cout << "== " << (allOk ? "ALL PASSED" : "SOME FAILED") << " ==\n";
}

// ---- command: register (the recipient publishes its bundle) ------------

static void doRegister(Store &store, const std::string &userId, const std::string &prekeyUrl,
                       const std::string &notaryUrl) {
	if (!store.hasIdentity()) {
		std::cout << "Not initialized. Run 'init' first.\n";
		return;
	}

	Identity id = store.loadIdentity();
	SignedPreKey spk = store.loadSignedPreKey();
	std::vector<OneTimePreKey> opks = store.loadOneTimePreKeys();

	uint64_t ts = (uint64_t)time(nullptr);
	Bytes core = serializeCore(id.edPub, id.xPub, spk.pub, spk.sig, ts);

	// 1) Build and POST the bundle to the PreKey server (path alpha).
	std::string opkArray = "[";
	for (size_t i = 0; i < opks.size(); i++) {
		opkArray += "\"" + acrypto::b64encode(opks[i].pub) + "\"";
		if (i + 1 < opks.size())
			opkArray += ",";
	}
	opkArray += "]";

	std::string bundle = "{";
	bundle += "\"identityKeyEd25519\":\"" + acrypto::b64encode(id.edPub) + "\",";
	bundle += "\"identityKeyX25519\":\"" + acrypto::b64encode(id.xPub) + "\",";
	bundle += "\"signedPreKey\":\"" + acrypto::b64encode(spk.pub) + "\",";
	bundle += "\"signedPreKeySignature\":\"" + acrypto::b64encode(spk.sig) + "\",";
	bundle += "\"timestamp\":" + std::to_string(ts) + ",";
	bundle += "\"oneTimePreKeys\":" + opkArray;
	bundle += "}";

	// 2) Build one attestation per one-time PreKey, keyed by id, POST to Notary (path beta).
	std::string atts = "{";
	for (size_t i = 0; i < opks.size(); i++) {
		Bytes digest = attestationDigest(core, opks[i].pub);
		Bytes att = acrypto::ed25519_sign(id.edPriv, digest);
		atts += "\"" + std::to_string(opks[i].id) + "\":\"" + acrypto::b64encode(att) + "\"";
		if (i + 1 < opks.size())
			atts += ",";
	}
	atts += "}";

	// Send both posts on the two paths at the same time (separate threads), so an
	// attacker cannot trivially synchronise a man-in-the-middle on both at once.
	auto fa = std::async(std::launch::async,
	    [&] { return httpPostJson(prekeyUrl + "/public/prekey/" + userId, bundle); });
	auto fb = std::async(std::launch::async,
	    [&] { return httpPostJson(notaryUrl + "/public/notary/" + userId, atts); });
	HttpResponse r1 = fa.get();
	HttpResponse r2 = fb.get();

	std::cout << "Registered '" << userId << "':\n";
	std::cout << "  PreKey server (alpha): HTTP " << r1.status
	          << (r1.error.empty() ? "" : " [" + r1.error + "]") << "\n";
	std::cout << "  Notary server (beta):  HTTP " << r2.status
	          << (r2.error.empty() ? "" : " [" + r2.error + "]") << " (" << opks.size() << " attestations)\n";
	std::cout << "  Pinned identity (Ed25519) for peers: " << acrypto::b64encode(id.edPub) << "\n";
}

// ---- fetch + verify a peer's bundle (shared by retrieve and send) ------

struct VerifiedPeer {
	bool ok = false;
	Bytes xPub, spkPub, opkPub;   // peer X25519 identity, signed prekey, one-time prekey
	std::string opkId;
};

// Fetches the bundle (path alpha) and the attestation set (path beta) at the
// same time, then verifies everything against the pinned identity key. Prints
// the four checks and returns the verified key material.
static VerifiedPeer fetchAndVerify(const std::string &peerId, const std::string &pinnedEdB64,
                                   const std::string &prekeyUrl, const std::string &notaryUrl) {
	VerifiedPeer vp;

	auto fb = std::async(std::launch::async,
	    [&] { return httpGet(prekeyUrl + "/public/prekey/" + peerId); });
	auto fa = std::async(std::launch::async,
	    [&] { return httpGet(notaryUrl + "/public/notary/" + peerId); });
	HttpResponse rb = fb.get();
	HttpResponse ra = fa.get();

	if (rb.status != 200) {
		std::cout << "Could not fetch bundle (HTTP " << rb.status << "). "
		          << "The peer may be unregistered or out of one-time prekeys.\n";
		return vp;
	}
	if (ra.status != 200) {
		std::cout << "Could not fetch attestations (HTTP " << ra.status << ").\n";
		return vp;
	}

	Bytes edPub = acrypto::b64decode(jsonStr(rb.body, "identityKeyEd25519"));
	Bytes xPub = acrypto::b64decode(jsonStr(rb.body, "identityKeyX25519"));
	Bytes spkPub = acrypto::b64decode(jsonStr(rb.body, "signedPreKey"));
	Bytes spkSig = acrypto::b64decode(jsonStr(rb.body, "signedPreKeySignature"));
	long ts = jsonLong(rb.body, "timestamp");
	std::string opkB64 = jsonStr(rb.body, "oneTimePreKey");
	std::string opkId = jsonStr(rb.body, "oneTimePreKeyId");
	Bytes opkPub = acrypto::b64decode(opkB64);

	// Pick the attestation matching the one-time prekey we received.
	Bytes attestation = acrypto::b64decode(jsonStr(ra.body, opkId));

	// Verify against the PINNED identity key (obtained out-of-band / TOFU).
	Bytes pinnedEd = acrypto::b64decode(pinnedEdB64);
	bool idMatches = (acrypto::b64encode(edPub) == pinnedEdB64);
	bool spkOk = acrypto::ed25519_verify(pinnedEd, spkPub, spkSig);
	Bytes core = serializeCore(edPub, xPub, spkPub, spkSig, (uint64_t)ts);
	Bytes digest = attestationDigest(core, opkPub);
	bool attOk = acrypto::ed25519_verify(pinnedEd, digest, attestation);
	long age = (long)time(nullptr) - ts;
	bool fresh = (ts > 0 && age >= 0 && age < FRESHNESS_WINDOW);
	bool verified = idMatches && spkOk && attOk && fresh;

	std::cout << "Bundle for '" << peerId << "' (one-time prekey id " << opkId << "):\n";
	std::cout << "  identity pin match: " << (idMatches ? "OK" : "FAIL") << "\n";
	std::cout << "  signed prekey sig:  " << (spkOk ? "OK" : "FAIL") << "\n";
	std::cout << "  attestation:        " << (attOk ? "OK" : "FAIL") << "\n";
	std::cout << "  freshness:          " << (fresh ? "OK" : "FAIL") << " (age " << age << "s)\n";
	std::cout << "  => BUNDLE " << (verified ? "VERIFIED" : "REJECTED") << "\n";

	if (verified) {
		vp.ok = true;
		vp.xPub = xPub;
		vp.spkPub = spkPub;
		vp.opkPub = opkPub;
		vp.opkId = opkId;
	}
	return vp;
}

// ---- command: retrieve (verify a peer's bundle, no key agreement) ------

static void doRetrieve(const std::string &peerId, const std::string &pinnedEdB64,
                       const std::string &prekeyUrl, const std::string &notaryUrl) {
	VerifiedPeer vp = fetchAndVerify(peerId, pinnedEdB64, prekeyUrl, notaryUrl);
	if (vp.ok) {
		std::cout << "  peer X25519 identity: " << acrypto::b64encode(vp.xPub) << "\n";
		std::cout << "  peer signed prekey:   " << acrypto::b64encode(vp.spkPub) << "\n";
		std::cout << "  peer one-time prekey: " << acrypto::b64encode(vp.opkPub) << "\n";
	}
}

// ---- command: send (initiator: verify peer, X3DH, encrypt, enqueue) ----

static void doSend(Store &store, const std::string &myId, const std::string &peerId,
                   const std::string &pinnedEdB64, const std::string &message,
                   const std::string &prekeyUrl, const std::string &notaryUrl) {
	if (!store.hasIdentity()) {
		std::cout << "Not initialized. Run 'init' first.\n";
		return;
	}
	Identity me = store.loadIdentity();

	VerifiedPeer vp = fetchAndVerify(peerId, pinnedEdB64, prekeyUrl, notaryUrl);
	if (!vp.ok) {
		std::cout << "Aborting send: peer bundle did not verify.\n";
		return;
	}

	// X3DH (initiator side): four Diffie-Hellman values mixed into one secret.
	Bytes ekPriv, ekPub;
	acrypto::x25519_keypair(ekPriv, ekPub);
	Bytes ikm;
	append(ikm, acrypto::x25519_dh(me.xPriv, vp.spkPub)); // DH1 = DH(IK_A, SPK_B)
	append(ikm, acrypto::x25519_dh(ekPriv, vp.xPub));     // DH2 = DH(EK_A, IK_B)
	append(ikm, acrypto::x25519_dh(ekPriv, vp.spkPub));   // DH3 = DH(EK_A, SPK_B)
	append(ikm, acrypto::x25519_dh(ekPriv, vp.opkPub));   // DH4 = DH(EK_A, OPK_B)
	Bytes sk = acrypto::kdf(ikm);
	Bytes aesKey(sk.begin(), sk.begin() + 32);

	// Encrypt the first message under SK.
	Bytes iv = acrypto::randbytes(12);
	Bytes ct = acrypto::aesgcm_encrypt(aesKey, iv, Bytes(message.begin(), message.end()));

	// Message payload (header + ciphertext); Base64 the whole thing so it travels
	// safely as the queue message content.
	std::string payload = "{";
	payload += "\"from\":\"" + myId + "\",";
	payload += "\"identityKeyX25519\":\"" + acrypto::b64encode(me.xPub) + "\",";
	payload += "\"ephemeralKey\":\"" + acrypto::b64encode(ekPub) + "\",";
	payload += "\"oneTimePreKeyId\":\"" + vp.opkId + "\",";
	payload += "\"iv\":\"" + acrypto::b64encode(iv) + "\",";
	payload += "\"ciphertext\":\"" + acrypto::b64encode(ct) + "\"";
	payload += "}";
	std::string payloadB64 = acrypto::b64encode(Bytes(payload.begin(), payload.end()));

	HttpResponse r = httpPostJson(prekeyUrl + "/public/send-message/" + myId + "/" + peerId, payloadB64);
	store.saveSession(peerId, sk);

	std::cout << "Sent encrypted message to '" << peerId << "' (queue HTTP " << r.status << ").\n";
	std::cout << "  session key SK: " << acrypto::b64encode(sk) << "\n";
}

// ---- command: fetch (recipient: drain queue, X3DH, decrypt) ------------

static void doFetch(Store &store, const std::string &myId, const std::string &prekeyUrl) {
	if (!store.hasIdentity()) {
		std::cout << "Not initialized. Run 'init' first.\n";
		return;
	}
	Identity me = store.loadIdentity();
	SignedPreKey spk = store.loadSignedPreKey();

	HttpResponse r = httpGet(prekeyUrl + "/public/messages/" + myId);
	if (r.status != 200) {
		std::cout << "Could not fetch messages (HTTP " << r.status << ").\n";
		return;
	}

	std::vector<std::string> payloads = jsonStringArray(r.body);
	if (payloads.empty()) {
		std::cout << "No messages.\n";
		return;
	}

	for (const std::string &pB64 : payloads) {
		Bytes payloadBytes = acrypto::b64decode(pB64);
		std::string payload(payloadBytes.begin(), payloadBytes.end());

		std::string from = jsonStr(payload, "from");
		Bytes peerIKx = acrypto::b64decode(jsonStr(payload, "identityKeyX25519"));
		Bytes ekPub = acrypto::b64decode(jsonStr(payload, "ephemeralKey"));
		std::string opkId = jsonStr(payload, "oneTimePreKeyId");
		Bytes iv = acrypto::b64decode(jsonStr(payload, "iv"));
		Bytes ct = acrypto::b64decode(jsonStr(payload, "ciphertext"));

		OneTimePreKey opk = store.loadOneTimePreKey(std::stoi(opkId));

		// X3DH (recipient side): mirror of the initiator's four DH values.
		Bytes ikm;
		append(ikm, acrypto::x25519_dh(spk.priv, peerIKx)); // DH1
		append(ikm, acrypto::x25519_dh(me.xPriv, ekPub));   // DH2
		append(ikm, acrypto::x25519_dh(spk.priv, ekPub));   // DH3
		append(ikm, acrypto::x25519_dh(opk.priv, ekPub));   // DH4
		Bytes sk = acrypto::kdf(ikm);
		Bytes aesKey(sk.begin(), sk.begin() + 32);

		Bytes pt = acrypto::aesgcm_decrypt(aesKey, iv, ct);
		if (pt.empty()) {
			std::cout << "  [decryption failed for a message from " << from << "]\n";
			continue;
		}

		store.saveSession(from, sk);
		store.markOneTimePreKeyUsed(std::stoi(opkId));

		std::cout << "Message from '" << from << "': " << std::string(pt.begin(), pt.end()) << "\n";
		std::cout << "  session key SK: " << acrypto::b64encode(sk) << "\n";
	}
}

// ---- entry point -------------------------------------------------------

static void usage(const char *prog) {
	std::cout << "Usage: " << prog << " <db-file> <command> [args]\n"
	          << "  init\n"
	          << "  show\n"
	          << "  selftest\n"
	          << "  register <userId> [prekeyUrl] [notaryUrl]\n"
	          << "  retrieve <peerId> <peerEd25519Pub-b64> [prekeyUrl] [notaryUrl]\n"
	          << "  send     <myId> <peerId> <peerEd25519Pub-b64> <message> [prekeyUrl] [notaryUrl]\n"
	          << "  fetch    <myId> [prekeyUrl]\n"
	          << "    (default prekeyUrl " << DEFAULT_PREKEY << ", notaryUrl " << DEFAULT_NOTARY << ")\n";
}

int main(int argc, char **argv) {
	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}
	std::string dbPath = argv[1];
	std::string cmd = argv[2];

	Store store(dbPath);
	store.initSchema();
	httpInit();
	int rc = 0;

	if (cmd == "init") {
		doInit(store);
	} else if (cmd == "show") {
		doShow(store);
	} else if (cmd == "selftest") {
		doSelfTest();
	} else if (cmd == "register") {
		if (argc < 4) {
			usage(argv[0]);
			rc = 1;
		} else {
			std::string userId = argv[3];
			std::string preUrl = argc > 4 ? argv[4] : DEFAULT_PREKEY;
			std::string notUrl = argc > 5 ? argv[5] : DEFAULT_NOTARY;
			doRegister(store, userId, preUrl, notUrl);
		}
	} else if (cmd == "retrieve") {
		if (argc < 5) {
			usage(argv[0]);
			rc = 1;
		} else {
			std::string peerId = argv[3];
			std::string pinned = argv[4];
			std::string preUrl = argc > 5 ? argv[5] : DEFAULT_PREKEY;
			std::string notUrl = argc > 6 ? argv[6] : DEFAULT_NOTARY;
			doRetrieve(peerId, pinned, preUrl, notUrl);
		}
	} else if (cmd == "send") {
		if (argc < 7) {
			usage(argv[0]);
			rc = 1;
		} else {
			std::string myId = argv[3];
			std::string peerId = argv[4];
			std::string pinned = argv[5];
			std::string message = argv[6];
			std::string preUrl = argc > 7 ? argv[7] : DEFAULT_PREKEY;
			std::string notUrl = argc > 8 ? argv[8] : DEFAULT_NOTARY;
			doSend(store, myId, peerId, pinned, message, preUrl, notUrl);
		}
	} else if (cmd == "fetch") {
		if (argc < 4) {
			usage(argv[0]);
			rc = 1;
		} else {
			std::string myId = argv[3];
			std::string preUrl = argc > 4 ? argv[4] : DEFAULT_PREKEY;
			doFetch(store, myId, preUrl);
		}
	} else {
		usage(argv[0]);
		rc = 1;
	}

	httpCleanup();
	return rc;
}
