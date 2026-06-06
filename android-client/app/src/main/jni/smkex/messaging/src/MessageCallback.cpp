#include "MessageCallback.h"
#include "MpBaseService.h"
#include "MpStatus.h"

#define THIS_TAG "MessageCallback"

MessageCallback::MessageCallback(JNIEnv *env) {
	MP_LOG1("Ctor:: Init Messages Callback");

	messageReceivedJNICallback = new JniCallback();
	messageReceivedJNICallback->init(env);

	messageSentJNICallback = new JniCallback();
	messageSentJNICallback->init(env);
}

MessageCallback* MessageCallback::getInstance(JNIEnv *env) {
	static MessageCallback instance(env);
	return &instance;
}

MessageCallback::~MessageCallback() {
}

JniCallback *MessageCallback::getMessageReceivedJNICallback() {
	return messageReceivedJNICallback;
}

JniCallback *MessageCallback::getMessageSentJNICallback() {
	return messageSentJNICallback;
}

void MessageCallback::onSendMsgDone(void* msgId, mp_status_t xferStatus) {
	int msgStatus = xferStatus == MP_MESSAGE_SENT ? 0 : -1;
	MP_LOG2_INT("Message sent with status ", msgStatus);
}

void MessageCallback::onMsgReceived(std::string const& serial, const uint8_t* msg, uint32_t msgLen) {
	MP_LOG1("New message received");
	messageReceivedJNICallback->notifyListenersWithGenericMessage(serial.c_str(), msg, msgLen);
}
