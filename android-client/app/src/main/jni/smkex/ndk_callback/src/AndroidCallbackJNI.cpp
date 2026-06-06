#include <jni.h>
#include <string>

#include "MpBaseService.h"
#include "MpSingleton.h"
#include "MpStatus.h"
#include "MpLogger.h"
#include "MpMsg.h"
#include "AccountRegCallback.h"
#include "SmkexKeyExchangedCallback.h"
#include "MessageCallback.h"
#include "CallsCallback.h"

extern "C" {

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_initJNICallbacks(JNIEnv *env, jobject instance) {
        
        /* Account Status Callback */
        static AccountRegCallback *regRcv = AccountRegCallback::getInstance(env);
        MpUserAccount* uc = MpService::instance()->getUserAccount();
        uc->addRegCallback(regRcv);

        /* Smkex key-exchanged Callback */
        static SmkexKeyExchangedCallback* smkexRcv = SmkexKeyExchangedCallback::getInstance(env);

        /* Messages Callback */
        static MessageCallback* msgRcv = MessageCallback::getInstance(env);
        MpService::instance()->getDataMsg()->addMsgCb(msgRcv);

        /* Voice Call callbacks */
        static CallsCallback *callRcv = CallsCallback::getInstance(env);
        MpService::instance()->getCallManager()->addCallCb(callRcv);

        return 0;
    }

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_removeJNICallbacks(JNIEnv *env, jobject instance) {
        static AccountRegCallback *regRcv =  AccountRegCallback::getInstance(env);
        regRcv->getCallback()->dismissListeners();

        static SmkexKeyExchangedCallback* smkexRcv = SmkexKeyExchangedCallback::getInstance(env);
        smkexRcv->getCallback()->dismissListeners();

        static MessageCallback* msgRcv = MessageCallback::getInstance(env);
        msgRcv->getMessageReceivedJNICallback()->dismissListeners();
        msgRcv->getMessageSentJNICallback()->dismissListeners();

        static CallsCallback *callRcv = CallsCallback::getInstance(env);
        callRcv->getReceivedCallJNICallback()->dismissListeners();
        callRcv->getCallStatusCallback()->dismissListeners();

        return 0;
    }


    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_subscribeToAccountStatus(JNIEnv *env, jobject instance, jobject listener) {
        jobject store_listener = env->NewGlobalRef(listener);
        jclass clazz = env->GetObjectClass(store_listener);
        jmethodID store_method = env->GetMethodID(clazz, "onAccountStatusReceived", "(I)V");

        static AccountRegCallback *regRcv =  AccountRegCallback::getInstance(env);
        regRcv->getCallback()->addListener(store_listener, store_method);
        MP_LOG1("Listener subscribed to Account Status: OK");

        return 0;
    }

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_subscribeToSmkexKeyExchangedStatus(JNIEnv *env, jobject instance, jobject listener) {
        jobject store_listener = env->NewGlobalRef(listener);
        jclass clazz = env->GetObjectClass(store_listener);
        jmethodID store_method = env->GetMethodID(clazz, "onKeyExchanged", "(I)V");

        static SmkexKeyExchangedCallback* smkexRcv = SmkexKeyExchangedCallback::getInstance(env);
        smkexRcv->getCallback()->addListener(store_listener, store_method);
        MP_LOG1("Listener subscribed to Smkex Key-Exchanged Status: OK");

        return 0;
    }

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_subscribeToSentMessages(JNIEnv *env, jobject instance, jobject listener) {
        jobject store_listener = env->NewGlobalRef(listener);
        jclass clazz = env->GetObjectClass(store_listener);
        jmethodID store_method = env->GetMethodID(clazz, "onMessageSent", "(I)V");

        static MessageCallback* msgRcv = MessageCallback::getInstance(env);
        msgRcv->getMessageSentJNICallback()->addListener(store_listener, store_method);
        MP_LOG1("Listener subscribed to Message-Sent Status: OK");

        return 0;
    }

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_subscribeToReceivedMessages(JNIEnv *env, jobject instance, jobject listener) {
        jobject store_listener = env->NewGlobalRef(listener);
        jclass clazz = env->GetObjectClass(store_listener);
        jmethodID store_method = env->GetMethodID(clazz, "onMessageReceived", "(Ljava/lang/String;[BI)V");

        static MessageCallback* msgRcv = MessageCallback::getInstance(env);
        msgRcv->getMessageReceivedJNICallback()->addListener(store_listener, store_method);
        MP_LOG1("Listener subscribed to Message-Received Status: OK");

        return 0;
    }

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_subscribeToReceivedCalls(JNIEnv *env, jobject instance, jobject listener) {
        jobject store_listener = env->NewGlobalRef(listener);
        jclass clazz = env->GetObjectClass(store_listener);
        jmethodID store_method = env->GetMethodID(clazz, "onReceivedCall", "(Ljava/lang/String;)V");

        static CallsCallback *callRcv = CallsCallback::getInstance(env);
        callRcv->getReceivedCallJNICallback()->addListener(store_listener, store_method);
        MP_LOG1("Listener subscribed to Call-Received Status: OK");

        return 0;
    }

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_subscribeToCallStatus(JNIEnv *env, jobject instance, jobject listener) {
        jobject store_listener = env->NewGlobalRef(listener);
        jclass clazz = env->GetObjectClass(store_listener);
        jmethodID store_method = env->GetMethodID(clazz, "onCallStatusReceived", "(I)V");

        static CallsCallback *callRcv = CallsCallback::getInstance(env);
        callRcv->getCallStatusCallback()->addListener(store_listener, store_method);
        MP_LOG1("Listener subscribed to Call Status: OK");

        return 0;
    }
}