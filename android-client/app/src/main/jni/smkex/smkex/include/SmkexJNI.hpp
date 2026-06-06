#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
// #include <map>
#include <jni.h>
#include <Smkex.h>

extern Smkex smkex;
extern JavaVM *jvm;

void sendDataViaChannel(const char * buddyId, const uint8_t *data, uint32_t len, int channel);
void initSmkexWebSockets(const char * clientId, int channel);

#endif