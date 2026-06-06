#include <jni.h>
#include <string>
#include <thread>
#include "base64.h"

#include "Smkex.h"
#include "SmkexSessionInfo.h"
#include "WebSocketsAndroid.hpp"
#include "SmkexKeyExchangedCallback.h"
#include "SmkexJNI.hpp"

#define SMKEX_SESSION_KEY_LEN 64

// Initialize JVM pointers and smkex pointers
JavaVM *jvm = nullptr;
Smkex smkex;

static void task_receiver(JNIEnv* env)
{
        MP_LOG1("In thread RECEIVER");

        /* Get JNI callback */
        SmkexKeyExchangedCallback *keyExchangedCallback = SmkexKeyExchangedCallback::getInstance(env);

        /* Wait until session is established */
        while(!smkex.isKeyEstablished())
        {
            // MP_LOG1("RECEIVER:: Waiting for session to be established.");
            usleep(10 * 1000);

            /* Read SMKEX messages if available */
            // MP_LOG1("RECEIVER:: Checking for new messages from web socket");
            if(smkex.checkNewMessages()) {
                MP_LOG1("RECEIVER:: Error retrieving SMKEX messages");
                keyExchangedCallback->onKeyExchangedFailed(-1);
            }
        }

        /* Get information on last established session */
        std::string lastBuddy = smkex.getLastEstablishedBuddyID();
        SmkexSessionInfo &session = smkex.getSessionInfo(lastBuddy);

        unsigned char kbuf[SMKEX_SESSION_KEY_LEN];
        unsigned int klen = session.getSessionKey(kbuf);

        MP_LOG2_INT("RECEIVER:: Session with buddy established. Session key length: ", klen);
        MP_LOG2_HEX("RECEIVER:: Session Key is: ", kbuf, klen);

        keyExchangedCallback->onKeyExchangedSucceeded();
}

void initSmkexWebSocketsHelper(JNIEnv * env, const char * clientId, int channel){
    jstring arg = env->NewStringUTF(clientId);

    std::string classFullyQualifiedPath = "org/certsign/smkex/networking/SmkexNetworkingAPI";
    jclass clazz = env->FindClass(classFullyQualifiedPath.c_str());
    if (clazz == nullptr){
        MP_LOG1("Found nullptr in findclass, exiting...\n");
        exit(EXIT_FAILURE);
    }

    // This is required for calling NewObject()
    jmethodID altmethod = env->GetMethodID(clazz, "<init>","()V");

    try {
        jobject obj = env->NewGlobalRef(env->NewObject(clazz, altmethod));

        // We don't need to free NewStringUTF methods since it will free automatically
        // when the stack frame ends
        jmethodID initWebSockets = env->GetMethodID(clazz, "initWebSockets", "(Ljava/lang/String;I)V");
        if(initWebSockets == NULL){
            MP_LOG1("Cannot find method id initWebSockets!\n");
            exit(EXIT_FAILURE);
        }

        env->CallVoidMethod(obj, initWebSockets, arg, channel);

        // Delete global objects
        env->DeleteGlobalRef(obj);

    } catch (const std::exception &e){
        MP_LOG2("Exception found!", e.what());
        exit(EXIT_FAILURE);
    }
}

void initSmkexWebSockets(const char * clientId, int channel) {
    if(jvm == NULL)
        return;

    JNIEnv * env;
    int getEnvStat = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);

    if (getEnvStat == JNI_EDETACHED) {
        if(jvm->AttachCurrentThread(&env, NULL) != 0)
            return;

        initSmkexWebSocketsHelper(env, clientId, channel);

        jvm->DetachCurrentThread();
    } else if (getEnvStat == JNI_OK) {
        initSmkexWebSocketsHelper(env, clientId, channel);
    }
}

void sendDataViaChannelHelper(JNIEnv * env, const char * buddyId, const uint8_t *data, uint32_t len, int channel) {
    jstring arg = env->NewStringUTF(buddyId);

    // Convert stream to base64
    std::string datab64 = base64_encode(data, len);
    uint32_t encodedLen = datab64.size();

    std::string classFullyQualifiedPath = "org/certsign/smkex/networking/SmkexNetworkingAPI";
    jclass clazz = env->FindClass(classFullyQualifiedPath.c_str());
    if (clazz == nullptr){
        MP_LOG1("Found nullptr in findclass, exiting...\n");
        exit(EXIT_FAILURE);
    }

    jmethodID altmethod = env->GetMethodID(clazz, "<init>","()V");

    try {
        jobject obj = env->NewGlobalRef(env->NewObject(clazz, altmethod));
        jmethodID sendWIFI = env->GetMethodID(clazz, "sendOverChannel", "(Ljava/lang/String;[BII)V");

        if(sendWIFI == NULL){
            MP_LOG1("Cannot find method id sendOverChannel!\n");
            exit(EXIT_FAILURE);
        }

        jbyteArray jBytes;
        jint jBytesLen = 0;
        jint jChannel = (jint) channel;
        if(!datab64.empty() && encodedLen > 0) {
            jBytes = env->NewByteArray(encodedLen);
            env->SetByteArrayRegion(jBytes, 0, encodedLen, (jbyte *)datab64.c_str());
            jBytesLen = (jint) encodedLen;
        }

        env->CallVoidMethod(obj, sendWIFI, arg, jBytes, jBytesLen, jChannel);

        env->DeleteGlobalRef(obj);
    } catch (const std::exception &e) {
        MP_LOG2("Exception found!", e.what());
        exit(EXIT_FAILURE);
    }
}

void sendDataViaChannel(const char * buddyId, const uint8_t *data, uint32_t len, int channel) {
    if(jvm == NULL)
        return;

    JNIEnv * env;
    int getEnvStat = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);

    if (getEnvStat == JNI_EDETACHED) {
        if(jvm->AttachCurrentThread(&env, NULL) != 0)
            return;

        sendDataViaChannelHelper(env, buddyId, data, len, channel);

        jvm->DetachCurrentThread();
    } else if (getEnvStat == JNI_OK) {
        sendDataViaChannelHelper(env, buddyId, data, len, channel);
    }
}

extern "C" {

    JNIEXPORT void JNICALL 
    Java_org_certsign_smkex_jni_SmkexJNI_init(JNIEnv* env, jobject thiz)
    {
        MP_LOG1("In init function\n");
        jint rs = env->GetJavaVM(&jvm);
        assert (rs == JNI_OK);
    }

    // Need to call OnMsgReceived here
    JNIEXPORT void JNICALL 
    Java_org_certsign_smkex_jni_SmkexJNI_onMsgReceived(JNIEnv* env, jobject thiz, jbyteArray data, jint messageLen, jint channel)
    {
        uint32_t dataLen = messageLen;
        int _channel = channel;
        int len = env->GetArrayLength (data);

        std::vector<char> buff(len + 1, 0);
        env->GetByteArrayRegion (data, 0, len, reinterpret_cast<jbyte*>(buff.data()));

        std::string sdata = base64_decode(buff.data());
        uint32_t decodedLen = sdata.size();
        smkex.processSmkexMessage((const uint8_t*)sdata.c_str(), decodedLen, _channel);
    }

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_initSmkexWS(JNIEnv* env, jobject thiz, jstring clientIdStr, jstring buddyIdStr) {
        MP_LOG1("Init Smkex WS");
        const char *clientId = env->GetStringUTFChars(clientIdStr, 0);
        const char *buddyId = env->GetStringUTFChars(buddyIdStr, 0);
        /* Setup */
        std::string clientId1(clientId);
        std::string clientId2 = clientId1 + "2";

        std::string buddyId1(buddyId);
        std::string buddyId2 = buddyId1 + "2";
        const std::string clientids[]= {clientId1, clientId2};
        const std::string buddyIds[]= {buddyId1, buddyId2};

        /* Configure Smkex channels ids */
        MP_LOG1("Trying to initialize connection via WS");
        smkex.setClientID(clientId1);
        smkex.setClientID2(clientId2);

        /* Initialize connection */
        WebSocketsAndroid &webSocketTransport = WebSocketsAndroid::getInstance();
        webSocketTransport.init(clientids, buddyIds, 2);
        webSocketTransport.addMsgCb(&smkex);

        smkex.setSmkexTransport(&webSocketTransport);

        MP_LOG1("Connection initialized!");

		/* Add SMKEX class as callback for OOB key setup functions */
		MpService::instance()->getSIPStack()->addOobKeySetup(&smkex);

		/* Start receiver thread */
		std::thread{task_receiver, env}.detach();

        return 0;
    }
}
