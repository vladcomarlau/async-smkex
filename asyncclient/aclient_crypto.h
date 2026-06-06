#ifndef ACLIENT_CRYPTO_H
#define ACLIENT_CRYPTO_H

#include <vector>
#include <string>

// All keys and signatures are handled as raw byte vectors.
typedef std::vector<unsigned char> Bytes;

/*
 * Minimal crypto helpers for the asynchronous SMKEX client.
 *
 * The symmetric primitives (SHA-256, AES-GCM, the HMAC-KDF, the RNG) are reused
 * from the existing SMKEX module (crypto.cpp). This file only adds the
 * asymmetric primitives the offline setting needs:
 *   - X25519 for the Diffie-Hellman steps (DH1, DH2, DH3),
 *   - Ed25519 for signing/verifying the signed PreKey and the attestations.
 * Both are thin wrappers over the OpenSSL EVP interface.
 */
namespace acrypto {

	// X25519 (Diffie-Hellman). Keys are 32 bytes each.
	void  x25519_keypair(Bytes &priv, Bytes &pub);
	Bytes x25519_dh(const Bytes &myPriv, const Bytes &peerPub);   // 32-byte shared secret

	// Ed25519 (signatures). Keys are 32 bytes, signatures 64 bytes.
	void  ed25519_keypair(Bytes &priv, Bytes &pub);
	Bytes ed25519_sign(const Bytes &priv, const Bytes &msg);
	bool  ed25519_verify(const Bytes &pub, const Bytes &msg, const Bytes &sig);

	// Helpers
	Bytes sha256(const Bytes &data);                 // reuses crypto.cpp
	Bytes randbytes(size_t n);
	std::string b64encode(const Bytes &data);
	Bytes b64decode(const std::string &s);

	// Symmetric primitives reused from crypto.cpp, wrapped for convenience.
	Bytes kdf(const Bytes &input);                                                    // 64-byte output (HMAC-SHA256 KDF)
	Bytes aesgcm_encrypt(const Bytes &key, const Bytes &iv, const Bytes &plaintext);  // key 32B, iv 12B
	Bytes aesgcm_decrypt(const Bytes &key, const Bytes &iv, const Bytes &ciphertext); // empty on failure

} // namespace acrypto

#endif
