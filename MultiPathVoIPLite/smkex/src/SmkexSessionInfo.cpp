/** \file
 *  \brief  SmkexSessionInfo.c source file
 *
 *  This file implements the core methods for a SMKEX Session 
 *
 *  Authors:
 *    Liliana Grigoriu (liliana.grigoriu@upb.ro)
 *    Marios O. Choudary (marios.choudary@cs.pub.ro)
 *
 *  Last update: August 2021
 */

#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include<stdio.h>
#include "Smkex.h"
#include "SmkexSessionInfo.h"

// functie de verificare din curs criptografie 
#define CHECK(assertion, call_description)  \
  do {                                      \
    if (!(assertion)) {                     \
      fprintf(stderr, "(%s, %d): ",         \
        __FILE__, __LINE__);                \
      perror(call_description);             \
      exit(EXIT_FAILURE);                   \
    }                                       \
  } while(0)

#define DEBUG 1
#define THIS_TAG "Smkex"

#ifndef __ANDROID__
#include "MpBaseService.h"
#include "MpLogger.h"
#define LOGMSG(x) MpService::instance()->getLogger()->print(THIS_TAG, __FUNCTION__, (x))
#else
#define LOGMSG(x)
#endif

int SmkexSessionInfo::_nextID = 0; 


// TODO: replace all the DH stuff into some function or place somewhere else
SmkexSessionInfo::SmkexSessionInfo(){

    // get unique ID
    _nextID++;
    _uniqueID = _nextID;
    _sessionID = _nextID;

#if DEBUG
    printf("In SmkexSessionInfo::Constructor with uniqueID=%d\n", _uniqueID);
#endif

    // Initialise protocol data
    memset(_session_key, 0, SMKEX_SESSION_KEY_LEN);
    _session_key_len = 0;
    memset(local_priv_key, 0, SMKEX_PRIV_KEY_LEN);
    local_priv_key_length = 0;
    memset(local_nonce, 0, SMKEX_NONCE_LEN);
    local_nonce_length = 0;
    memset(local_pub_key, 0, SMKEX_PUB_KEY_LEN);
    local_pub_key_length = 0;
    memset(remote_nonce, 0, SMKEX_NONCE_LEN);
    remote_nonce_length = 0;
    memset(remote_pub_key, 0, SMKEX_PUB_KEY_LEN);
    remote_pub_key_length = 0;

    // initialise DH structure 
#if DEBUG
    printf("Init DH parameters from file\n") ;
#endif
    //  currently computed and stored in _dh from "../smkex/dhparam.pem"
    // TODO: use EC instead
    __read_pg_from_file(SMKEX_DH_PARAMETER_FILENAME);

#if DEBUG
    printf("SmkexSessionInfo::Constructor ending here\n");
#endif
}

SmkexSessionInfo::~SmkexSessionInfo(){
#if DEBUG
    printf("In SmkexSessionInfo::Destructor with uniqueID=%d\n", _uniqueID);
#endif
    if (_dh!=0) free(_dh);
#if DEBUG
    printf("SmkexSessionInfo::Destructor ending here\n");
#endif
}

SmkexSessionInfo& SmkexSessionInfo::operator=(const SmkexSessionInfo& other)
{
#if DEBUG
  printf("We are in the copy constructor of SmkexSessionInfo\n");
#endif

  if (this == &other)
     return *this;

#if DEBUG
  printf("and in SmkexSessionInfo(&other) we are not called by *this.... \n");
#endif

  _iAmSessionInitiator = other.isInitiator();
  _state = other.getState();
  _sessionID = other.getSessionID();
  _buddy = other.getBuddy();
  _buddy2 = other.getBuddy2();
  __read_pg_from_file(SMKEX_DH_PARAMETER_FILENAME);
  if(initKeysfromDH())
    LOGMSG("Error initialising DH keys\n");
  // overwrite key values from given object
  local_priv_key_length = other.getLocalPrivKey(local_priv_key);
  local_pub_key_length = other.getLocalPubKey(local_pub_key);
  local_nonce_length = other.getLocalNonce(local_nonce);
  remote_pub_key_length = other.getRemotePubKey(remote_pub_key);
  remote_nonce_length = other.getRemoteNonce(remote_nonce);
  _session_key_len = other.getSessionKey(_session_key);

  return *this;
}

int SmkexSessionInfo::initKeysfromDH(void)
{

  if(_dh_initialised)
    LOGMSG("Warning: re-initialising DH keys\n");

  int _i= DH_generate_key(_dh);  // computes key values dh->priv_key and dh->pub_key

  if (_i!=1){
      printf("ERROR in  DH_generate_key\n exiting.\n") ;
      return -1;
  }
  LOGMSG("successfully generated local keys\n");

  BIGNUM *local_pub_key_num, *local_priv_key_num;
  
#if OPENSSL_VERSION_NUMBER < 0x10100000L // OpenSSL 1.0.2
  if (local_pub_key_num != NULL)
    local_pub_key_num = _dh->pub_key;
  if (local_priv_key_num != NULL)
    local_priv_key_num = _dh->priv_key;

#else // OpenSSL 1.1.1
  DH_get0_key(_dh, (const BIGNUM**) &local_pub_key_num,
      (const BIGNUM**) &local_priv_key_num);
#endif

  local_pub_key_length =  BN_num_bytes(local_pub_key_num);
  local_priv_key_length =  BN_num_bytes(local_priv_key_num);

  printf("Pub key has %d bytes\n", local_pub_key_length);    
  CHECK(SMKEX_PUB_KEY_LEN == local_pub_key_length, "DH PUB KEY LEN");    
  CHECK(SMKEX_PRIV_KEY_LEN == local_priv_key_length, "DH PRIV KEY LEN");

  BN_bn2bin(local_pub_key_num, local_pub_key);
  BN_bn2bin(local_priv_key_num, local_priv_key);

  _dh_initialised = true;

#if DEBUG
    printf("DH keys initialised.\n\nPublic key is: ");
    for(unsigned int k=0; k<local_pub_key_length; k++)
        printf("%02X", local_pub_key[k]);
    printf("\n");
    printf("Private key is: ");
    for(unsigned int k=0; k<local_priv_key_length; k++)
        printf("%02X", local_priv_key[k]);
    printf("\n\n");
#endif

  return 0;
}


/**
 * Open file <filename>, read public Diffie-Hellman parameters P and G and store them in <pdhm>
 * in dh (Diffie-Hellman key exchange context)
 * @param filename file from which to read P and G
 */
void SmkexSessionInfo::__read_pg_from_file(const char * filename) {
    BIO * pbio;
    /* Get DH modulus and generator (P and G) */
    pbio = BIO_new_file(filename, "r");

    CHECK(pbio != NULL, "BIO_new_file");

    /* Read P and G from f  and store in the attribute of this object */
    _dh = PEM_read_bio_DHparams(pbio, NULL, NULL, NULL);
    CHECK(_dh != NULL, "PEM_read_bio_DHparams");
    BIO_free(pbio);
}

int SmkexSessionInfo::generateLocalNonce(unsigned char nbuf[])
{
  // create nonce for this message
  if(RAND_bytes(local_nonce,SMKEX_NONCE_LEN)==0){
    printf("Error creating nonce...............\nExiting\n");
    return 0;
  }
  local_nonce_length=SMKEX_NONCE_LEN;

  if(nbuf != NULL)
    memcpy(nbuf, local_nonce, local_nonce_length);

  return local_nonce_length;
}

int SmkexSessionInfo::getLocalNonce(unsigned char nbuf[]) const
{
  if(nbuf != NULL)
    memcpy(nbuf, local_nonce, local_nonce_length);

  return local_nonce_length;
}

unsigned int SmkexSessionInfo::computeHash(unsigned char dest[])
{
  unsigned int len, hlen;
  unsigned char buf[2*SMKEX_NONCE_LEN + 2*SMKEX_PUB_KEY_LEN];
  len = 0;
  memcpy(buf, remote_nonce, remote_nonce_length); 
  len += remote_nonce_length;
  memcpy(&buf[len], remote_pub_key, remote_pub_key_length); 
  len += remote_pub_key_length;
  memcpy(&buf[len], local_nonce, local_nonce_length); 
  len += local_nonce_length;
  memcpy(&buf[len], local_pub_key, local_pub_key_length); 
  len += local_pub_key_length;
  hlen = compute_sha256(dest, buf, len);

#if DEBUG
  printf("In SmkexSessionInfo::computeHash, computed hash has %d bytes: \n", hlen);
  for(unsigned int k=0; k<hlen; k++)
    printf("%02X", dest[k]);
  printf("\n");
  printf("\nRemote nonce: \n");
  for(unsigned int k=0; k<remote_nonce_length; k++)
    printf("%02X", remote_nonce[k]);
  printf("\nRemote pub key: \n");
  for(unsigned int k=0; k<remote_pub_key_length; k++)
    printf("%02X", remote_pub_key[k]);
  printf("\nLocal nonce: \n");
  for(unsigned int k=0; k<local_nonce_length; k++)
    printf("%02X", local_nonce[k]);
  printf("\nLocal pub key: \n");
  for(unsigned int k=0; k<local_pub_key_length; k++)
    printf("%02X", local_pub_key[k]);
  printf("\n");
#endif

  return hlen;
}

bool SmkexSessionInfo::verifyHash(const unsigned char hbuf[], unsigned int hlen)
{
#if DEBUG
  printf("In SmkexSessionInfo::verifyHash\n");
  printf("Received hash has %d bytes: \n", hlen);
  for(unsigned int k=0; k<hlen; k++)
    printf("%02X", hbuf[k]);
  printf("\n"); 
#endif
  unsigned int len, hhlen;
  unsigned char hh[SMKEX_HASH_LEN];
  unsigned char buf[2*SMKEX_NONCE_LEN + 2*SMKEX_PUB_KEY_LEN];
  len = 0;
  memcpy(buf, local_nonce, local_nonce_length); 
  len += local_nonce_length;
  memcpy(&buf[len], local_pub_key, local_pub_key_length); 
  len += local_pub_key_length;
  memcpy(&buf[len], remote_nonce, remote_nonce_length); 
  len += remote_nonce_length;
  memcpy(&buf[len], remote_pub_key, remote_pub_key_length); 
  len += remote_pub_key_length;
  hhlen = compute_sha256(hh, buf, len);
#if DEBUG
  printf("Locally computed hash has %d bytes: \n", hhlen);
  for(unsigned int k=0; k<hhlen; k++)
    printf("%02X", hh[k]);
  printf("\n");
  printf("\nLocal nonce: \n");
  for(unsigned int k=0; k<local_nonce_length; k++)
    printf("%02X", local_nonce[k]);
  printf("\nLocal pub key: \n");
  for(unsigned int k=0; k<local_pub_key_length; k++)
    printf("%02X", local_pub_key[k]);
  printf("\nRemote nonce: \n");
  for(unsigned int k=0; k<remote_nonce_length; k++)
    printf("%02X", remote_nonce[k]);
  printf("\nRemote pub key: \n");
  for(unsigned int k=0; k<remote_pub_key_length; k++)
    printf("%02X", remote_pub_key[k]);
  printf("\n");
#endif
  if ((hhlen != hlen) || (strncmp((char*)hh, (char*)hbuf, hhlen) != 0))
    return false;

  return true;
}

// TODO: update with EC keys
int SmkexSessionInfo::computeSessionKey(unsigned char kbuf[])
{
  int dklen;
  unsigned char dhkey[SMKEX_DH_KEY_LEN];
  BIGNUM *pub_key_buddy = BN_bin2bn(remote_pub_key, SMKEX_PUB_KEY_LEN, NULL);
  dklen = DH_compute_key(dhkey, pub_key_buddy, _dh);
  BN_free(pub_key_buddy);
  if (dklen == 0)
    return 0;

  // Use a KDF so we get a uniform pseudorandom session key
  nist_800_kdf(dhkey, dklen, _session_key, &_session_key_len);

  if (kbuf != NULL)
    memcpy(kbuf, _session_key, _session_key_len);

  return _session_key_len;
}

int SmkexSessionInfo::getSessionKey(unsigned char kbuf[]) const
{
  if(kbuf != NULL)
    memcpy(kbuf, _session_key, _session_key_len);
  
  return _session_key_len;
}

int SmkexSessionInfo::getLocalPrivKey(unsigned char kbuf[]) const
{
  if(kbuf != NULL)
    memcpy(kbuf, local_priv_key, local_priv_key_length);
  
  return local_priv_key_length;
}

int SmkexSessionInfo::getLocalPubKey(unsigned char kbuf[]) const
{
  if(kbuf != NULL)
    memcpy(kbuf, local_pub_key, local_pub_key_length);
  
  return local_pub_key_length;
}

int SmkexSessionInfo::getRemotePubKey(unsigned char kbuf[]) const
{
  if(kbuf != NULL)
    memcpy(kbuf, remote_pub_key, remote_pub_key_length);
  
  return remote_pub_key_length;
}

int SmkexSessionInfo::getRemoteNonce(unsigned char nbuf[]) const
{
  if(nbuf != NULL)
    memcpy(nbuf, remote_nonce, remote_nonce_length);
  
  return remote_nonce_length;
}

void SmkexSessionInfo::printSessionInfo() const
{
    cout << "In SmkexSessionInfo::printSessionInfo() we have this data: " << endl;
    cout << "Unieque ID: " << _uniqueID << endl;
    cout << "Is Initiator: " << _iAmSessionInitiator << endl;
    cout << "Session ID: " << _sessionID << endl;
    cout << "Buddy ID: " << _buddy << endl;
    cout << "Buddy ID2: " << _buddy2 << endl;
    cout << "DH Initialised: " << _dh_initialised << endl;
    cout << "Session key len: " << _session_key_len << endl;
    cout << "Session key bytes: " << endl;
    for(int k=0; k < _session_key_len; k++)
      printf("%02X", _session_key[k]); //cout << hex << _session_key[k]; ???
    cout << endl;
}
