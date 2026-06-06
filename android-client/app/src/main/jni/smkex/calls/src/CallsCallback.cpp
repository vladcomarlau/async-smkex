#include "CallsCallback.h"
#include "MpBaseService.h"
#include "MpStatus.h"

#define THIS_TAG "CallsCallback"

CallsCallback::CallsCallback(JNIEnv *env) {
	MP_LOG1("Ctor:: Init Calls Callback");

	receivedCallJNICallback = new JniCallback();
	receivedCallJNICallback->init(env);

	callStatusCallback = new JniCallback();
	callStatusCallback->init(env);
}

CallsCallback* CallsCallback::getInstance(JNIEnv *env) {
	static CallsCallback instance(env);
	return &instance;
}

CallsCallback::~CallsCallback() {
}

JniCallback *CallsCallback::getReceivedCallJNICallback() {
	return receivedCallJNICallback;
}

JniCallback *CallsCallback::getCallStatusCallback() {
	return callStatusCallback;
}

void CallsCallback::call_caller_calling(pjsip_status_code last_call_status) {
	MP_LOG1("call_caller_calling");
	callStatusCallback->notifyListenersWithValue((jint) last_call_status);
}

void CallsCallback::call_callee_incoming(const char* caller_serial) {
	MP_LOG1("call_callee_incoming");
	receivedCallJNICallback->notifyListenersWithMessage(caller_serial);
}

void CallsCallback::call_caller_early(pjsip_status_code last_call_status) {
	MP_LOG1("call_caller_early");
	callStatusCallback->notifyListenersWithValue((jint) last_call_status);
}

void CallsCallback::call_u_connecting(pjsip_status_code last_call_status) {
	MP_LOG1("call_u_connecting");
	callStatusCallback->notifyListenersWithValue((jint) last_call_status);
}

void CallsCallback::call_u_confirmed(pjsip_status_code last_call_status) {
	MP_LOG1("call_u_confirmed");
	callStatusCallback->notifyListenersWithValue((jint) last_call_status);
}

void CallsCallback::call_u_disconnected(pjsip_status_code last_call_status) {
	MP_LOG1("call_u_disconnected");
	callStatusCallback->notifyListenersWithValue((jint) last_call_status);
}
