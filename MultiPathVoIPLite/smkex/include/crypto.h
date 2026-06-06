#ifndef CRYPTO_H
#define CRYPTO_H

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/hmac.h>

#define KDF_KEY_LENGTH 64
#define SESSION_KEY_LENGTH 32
#define SESSION_IV_LENGTH 12
#define SESSION_TAG_LENGTH 16
#define SESSION_NONCE_LENGTH 8


/*
 * @brief: computes SHA256 of given input
 *
 * @param[in] dest[]: preallocated buffer long enough to store result of SHA256 (32 bytes)
 * @param[in] src[]: source buffer containing data to be hashed
 * @param[in] dlen: length of data in src buffer
 * @returns: length of computed sha256 function (should be 32 if all ok).
 */
unsigned int compute_sha256(unsigned char dest[], const unsigned char src[], int dlen);

void hexdump(unsigned char * string, int length);
void sha1dump(unsigned char * string, int length);

int mp_aesgcm_encrypt(const unsigned char * ptext,
        size_t plen,
        const unsigned char * key,
        const unsigned char * iv,
        unsigned char * ctext,
        size_t * clen);

int mp_aesgcm_decrypt(const unsigned char * ctext,
        size_t clen,
        const unsigned char * key,
        const unsigned char * iv,
        unsigned char * ptext,
        size_t * plen);

int mp_randomize(unsigned char *buffer, int len);

/* KDF based of NIST SP 800-108 (HMAC + Counter)
 * Generates 64 pseudorandom bytes using HMAC-SHA-256
 * Basically out = HMAC(k, 0) || HMAC(k, 1)
 *
 * @param[in] in: input key
 * @parma[in] inlen: input key length
 * @param[in] out: buffer for hmac output
 * @param[in] outlen: length of resulting pseudorandom stream (should be 64)
 * @returns the output buffer, same as out
 */
void * nist_800_kdf(const void * in, unsigned int inlen, void * out, unsigned int * outlen);

EC_KEY* __new_key_pair(void);
#endif
