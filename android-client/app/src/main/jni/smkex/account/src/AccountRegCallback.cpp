#include "AccountRegCallback.h"
#include "MpBaseService.h"

#define THIS_TAG "AccountRegCallback"

AccountRegCallback::AccountRegCallback(JNIEnv *env) {
	MP_LOG1("Ctor:: Init Account Registration Callback");

	accountJNICallback = new JniCallback();
	accountJNICallback->init(env);
}

AccountRegCallback* AccountRegCallback::getInstance(JNIEnv *env) {
	static AccountRegCallback instance(env);
	return &instance;
}

AccountRegCallback::~AccountRegCallback() {
}

JniCallback *AccountRegCallback::getCallback() {
	return accountJNICallback;
}

void AccountRegCallback::registration_state(mp_online_state_t status) {
	if (status == MP_ONLINE) {
		MP_LOG1("Registration status ONLINE. Notify Java");
		accountJNICallback->notifyListenersWithValue(0);
	} else if (status == MP_OFFLINE) {
		MP_LOG1("Registration status OFFLINE. Notify Java");
		accountJNICallback->notifyListenersWithValue(1);
	} else {
		MP_LOG1("Registration status ERR. Notify Java");
		accountJNICallback->notifyListenersWithValue(-1);
	}
}

