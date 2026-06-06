#ifndef __JNI_CALLBACK_OBSERVER_CHAIN_H__
#define __JNI_CALLBACK_OBSERVER_CHAIN_H__

#include <jni.h>
#include <string.h>

class ObserverChain {
public:
    ObserverChain(jobject pJobject, jmethodID pID);

    jobject store_Wlistener=NULL;
    jmethodID store_method = NULL;
};


#endif /* __JNI_CALLBACK_OBSERVER_CHAIN_H__ */