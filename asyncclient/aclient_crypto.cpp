#include "aclient_crypto.h"
#include "crypto.h" // reused from MultiPathVoIPLite/smkex (compute_sha256, ...)

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <iostream>

namespace acrypto {

// ---- internal helpers --------------------------------------------------

// Generate a raw keypair for the given key type (EVP_PKEY_X25519 or _ED25519).
static void raw_keypair(int type, Bytes &priv, Bytes &pub) {
	EVP_PKEY *pkey = nullptr;
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(type, nullptr);
	EVP_PKEY_keygen_init(ctx);
	EVP_PKEY_keygen(ctx, &pkey);

	size_t len = 32;
	priv.resize(32);
	pub.resize(32);
	EVP_PKEY_get_raw_private_key(pkey, priv.data(), &len);
	len = 32;
	EVP_PKEY_get_raw_public_key(pkey, pub.data(), &len);

	EVP_PKEY_free(pkey);
	EVP_PKEY_CTX_free(ctx);
}

// ---- X25519 ------------------------------------------------------------

void x25519_keypair(Bytes &priv, Bytes &pub) {
	raw_keypair(EVP_PKEY_X25519, priv, pub);
}

Bytes x25519_dh(const Bytes &myPriv, const Bytes &peerPub) {
	Bytes secret;

	EVP_PKEY *mine = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr, myPriv.data(), myPriv.size());
	EVP_PKEY *theirs = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr, peerPub.data(), peerPub.size());

	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(mine, nullptr);
	EVP_PKEY_derive_init(ctx);
	EVP_PKEY_derive_set_peer(ctx, theirs);

	size_t len = 0;
	EVP_PKEY_derive(ctx, nullptr, &len);
	secret.resize(len);
	EVP_PKEY_derive(ctx, secret.data(), &len);
	secret.resize(len);

	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(mine);
	EVP_PKEY_free(theirs);
	return secret;
}

// ---- Ed25519 -----------------------------------------------------------

void ed25519_keypair(Bytes &priv, Bytes &pub) {
	raw_keypair(EVP_PKEY_ED25519, priv, pub);
}

Bytes ed25519_sign(const Bytes &priv, const Bytes &msg) {
	Bytes sig;

	EVP_PKEY *key = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, priv.data(), priv.size());
	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, key);

	size_t siglen = 0;
	EVP_DigestSign(ctx, nullptr, &siglen, msg.data(), msg.size());
	sig.resize(siglen);
	EVP_DigestSign(ctx, sig.data(), &siglen, msg.data(), msg.size());
	sig.resize(siglen);

	EVP_MD_CTX_free(ctx);
	EVP_PKEY_free(key);
	return sig;
}

bool ed25519_verify(const Bytes &pub, const Bytes &msg, const Bytes &sig) {
	EVP_PKEY *key = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, pub.data(), pub.size());
	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, key);

	int rc = EVP_DigestVerify(ctx, sig.data(), sig.size(), msg.data(), msg.size());

	EVP_MD_CTX_free(ctx);
	EVP_PKEY_free(key);
	return rc == 1;
}

// ---- helpers -----------------------------------------------------------

Bytes sha256(const Bytes &data) {
	Bytes out(32);
	compute_sha256(out.data(), data.data(), (int)data.size()); // from crypto.cpp
	return out;
}

Bytes randbytes(size_t n) {
	Bytes out(n);
	RAND_bytes(out.data(), (int)n);
	return out;
}

std::string b64encode(const Bytes &data) {
	if (data.empty())
		return "";
	int enclen = 4 * ((int)(data.size() + 2) / 3);
	std::string out(enclen, '\0');
	int n = EVP_EncodeBlock((unsigned char *)out.data(), data.data(), (int)data.size());
	out.resize(n);
	return out;
}

Bytes b64decode(const std::string &s) {
	if (s.empty())
		return {};
	Bytes out(3 * (s.size() / 4));
	int n = EVP_DecodeBlock(out.data(), (const unsigned char *)s.data(), (int)s.size());
	if (n < 0)
		return {};
	// EVP_DecodeBlock always returns a multiple of 3; trim the '=' padding.
	int pad = 0;
	if (s.size() >= 1 && s[s.size() - 1] == '=')
		pad++;
	if (s.size() >= 2 && s[s.size() - 2] == '=')
		pad++;
	out.resize(n - pad);
	return out;
}

// HMAC-SHA256 based key-derivation function (reused from crypto.cpp), 64 bytes.
Bytes kdf(const Bytes &input) {
	Bytes out(64);
	unsigned int outlen = 0;
	nist_800_kdf(input.data(), (unsigned int)input.size(), out.data(), &outlen);
	out.resize(outlen);
	return out;
}

// AES-256-GCM (reused from crypto.cpp). Output is ciphertext || 16-byte tag.
Bytes aesgcm_encrypt(const Bytes &key, const Bytes &iv, const Bytes &plaintext) {
	Bytes ct(plaintext.size() + 16);
	size_t clen = 0;
	if (mp_aesgcm_encrypt(plaintext.data(), plaintext.size(), key.data(), iv.data(), ct.data(), &clen) != 1)
		return {};
	ct.resize(clen);
	return ct;
}

Bytes aesgcm_decrypt(const Bytes &key, const Bytes &iv, const Bytes &ciphertext) {
	if (ciphertext.size() < 16)
		return {};
	Bytes pt(ciphertext.size());
	size_t plen = 0;
	if (mp_aesgcm_decrypt(ciphertext.data(), ciphertext.size(), key.data(), iv.data(), pt.data(), &plen) != 1)
		return {};
	pt.resize(plen);
	return pt;
}

} // namespace acrypto
