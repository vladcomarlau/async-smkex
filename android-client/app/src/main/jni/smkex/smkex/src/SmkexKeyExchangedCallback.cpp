#include "SmkexKeyExchangedCallback.h"
#define THIS_TAG "SmkexKeyExchangedCallback"

SmkexKeyExchangedCallback::SmkexKeyExchangedCallback(JNIEnv *env) {
	MP_LOG1("Ctor:: Init Smkex Key-Exchanged Callback");

	smkexJNICallback = new JniCallback();
	smkexJNICallback->init(env);
}

SmkexKeyExchangedCallback* SmkexKeyExchangedCallback::getInstance(JNIEnv *env) {
	static SmkexKeyExchangedCallback instance(env);
	return &instance;
}

SmkexKeyExchangedCallback::~SmkexKeyExchangedCallback() {
}

JniCallback *SmkexKeyExchangedCallback::getCallback() {
	return smkexJNICallback;
}

void SmkexKeyExchangedCallback::onKeyExchangedFailed(int status) {
	smkexJNICallback->notifyListenersWithValue(status);	
}

void SmkexKeyExchangedCallback::onKeyExchangedSucceeded() {
	smkexJNICallback->notifyListenersWithValue(0);
}