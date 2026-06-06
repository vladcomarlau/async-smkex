#include "JniCallback.h"


JavaVM *JniCallback::jvm = NULL;

/**
 * Constructor 
 */
JniCallback::JniCallback() {
}


/** 
 * Destructor 
 */
JniCallback::~JniCallback() {
}

/**
 * Init JNI callback 
 */
void JniCallback::init(JNIEnv *env) {
    env->GetJavaVM(&jvm);
}

/**
 * Remove all listeners
 */
void JniCallback::dismissListeners() {
    JNIEnv * env;
    int getEnvStat = JniCallback::jvm->GetEnv((void **) &env, JNI_VERSION_1_6);

	if (!store_Wlistener_vector.empty()) {
        for (int i = 0; i < store_Wlistener_vector.size(); i++) {
            env->DeleteGlobalRef(store_Wlistener_vector[i]->store_Wlistener);
            store_Wlistener_vector[i]->store_method = NULL;
        }
        store_Wlistener_vector.clear();
    }
}

/**
 * Add listener
 */
void JniCallback::addListener(jweak store_Wlistener, jmethodID store_method) {
	ObserverChain *tmpt = new ObserverChain(store_Wlistener, store_method);
    store_Wlistener_vector.push_back(tmpt);
}

/**
 * Notify listener with structure {String, ByteArray, BytesLength}
 */
void JniCallback::notifyListenersWithGenericMessage(const char* str, const uint8_t* bytes, uint32_t bytesLen) {
    if(JniCallback::jvm == NULL) 
        return;

    JNIEnv * env;
    int getEnvStat = JniCallback::jvm->GetEnv((void **) &env, JNI_VERSION_1_6);

    if (getEnvStat == JNI_EDETACHED) {
        if(JniCallback::jvm->AttachCurrentThread(&env, NULL) != 0)
            return; 

         // Cast params to JNI required data types
        jstring jStr;
        if(str != NULL)
            jStr = env->NewStringUTF(str);
    
        jbyteArray jBytes;
        jint jBytesLen = 0;
        if(bytes != NULL && bytesLen > 0) {
            jBytes = env->NewByteArray(bytesLen);
            env->SetByteArrayRegion(jBytes, 0, bytesLen, (jbyte *)bytes);
            jBytesLen = (jint) bytesLen;
        }
        
        if (!store_Wlistener_vector.empty()) {
            for (int i = 0; i < store_Wlistener_vector.size(); i++) {
                if(store_Wlistener_vector[i]->store_method != NULL) {
                    env->CallVoidMethod(
                        store_Wlistener_vector[i]->store_Wlistener,
                        store_Wlistener_vector[i]->store_method,
                        jStr,
                        jBytes,
                        jBytesLen
                    );
                }
            }
        }

        JniCallback::jvm->DetachCurrentThread();
    } else if (getEnvStat == JNI_OK) {

        // Cast params to JNI required data types
        jstring jStr;
        if(str != NULL)
            jStr = env->NewStringUTF(str);
    
        jbyteArray jBytes;
        jint jBytesLen = 0;
        if(bytes != NULL && bytesLen > 0) {
            jBytes = env->NewByteArray(bytesLen);
            env->SetByteArrayRegion(jBytes, 0, bytesLen, (jbyte *)bytes);
            jBytesLen = (jint) bytesLen;
        }

        if (!store_Wlistener_vector.empty()) {
            for (int i = 0; i < store_Wlistener_vector.size(); i++) {
                if(store_Wlistener_vector[i]->store_method != NULL) {
                    env->CallVoidMethod(
                        store_Wlistener_vector[i]->store_Wlistener,
                        store_Wlistener_vector[i]->store_method,
                        jStr,
                        jBytes,
                        jBytesLen
                    );
                }
            }
        }
    }
}

/**
 * Notify listener with integer message 
 */
void JniCallback::notifyListenersWithValue(jint messageVal_) {
    if(JniCallback::jvm == NULL) 
        return;

    JNIEnv * env;
    int getEnvStat = JniCallback::jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    
    if (getEnvStat == JNI_EDETACHED) {
        if(JniCallback::jvm->AttachCurrentThread(&env, NULL) != 0)
            return; 

        if (!store_Wlistener_vector.empty()) {
            for (int i = 0; i < store_Wlistener_vector.size(); i++) {
                if(store_Wlistener_vector[i]->store_method != NULL) {
                    env->CallVoidMethod(
                        store_Wlistener_vector[i]->store_Wlistener, // Object 
                        store_Wlistener_vector[i]->store_method, // Method 
                        messageVal_ // Params 
                    );
                }
            }
        }

        JniCallback::jvm->DetachCurrentThread();
    } else if (getEnvStat == JNI_OK) {
         if (!store_Wlistener_vector.empty()) {
            for (int i = 0; i < store_Wlistener_vector.size(); i++) {
                if(store_Wlistener_vector[i]->store_method != NULL) {
                    env->CallVoidMethod(
                        store_Wlistener_vector[i]->store_Wlistener,
                        store_Wlistener_vector[i]->store_method,
                        messageVal_
                    );
                }
            }
        }
    }
}

/**
 * Notify listene with a char* message 
 */
void JniCallback::notifyListenersWithMessage(const char* str) {
     if(JniCallback::jvm == NULL) 
        return;

    JNIEnv * env;
    int getEnvStat = JniCallback::jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    
    if (getEnvStat == JNI_EDETACHED) {
        if(JniCallback::jvm->AttachCurrentThread(&env, NULL) != 0)
            return; 

        jstring jStr;
        if(str != NULL)
            jStr = env->NewStringUTF(str);

        if (!store_Wlistener_vector.empty()) {
            for (int i = 0; i < store_Wlistener_vector.size(); i++) {
                if(store_Wlistener_vector[i]->store_method != NULL) {
                    env->CallVoidMethod(
                        store_Wlistener_vector[i]->store_Wlistener, // Object 
                        store_Wlistener_vector[i]->store_method, // Method 
                        jStr // Params 
                    );
                }
            }
        }

        JniCallback::jvm->DetachCurrentThread();
    } else if (getEnvStat == JNI_OK) {

        jstring jStr;
        if(str != NULL)
            jStr = env->NewStringUTF(str);

         if (!store_Wlistener_vector.empty()) {
            for (int i = 0; i < store_Wlistener_vector.size(); i++) {
                if(store_Wlistener_vector[i]->store_method != NULL) {
                    env->CallVoidMethod(
                        store_Wlistener_vector[i]->store_Wlistener,
                        store_Wlistener_vector[i]->store_method,
                        jStr
                    );
                }
            }
        }
    }
}