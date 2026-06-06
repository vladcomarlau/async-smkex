#ifndef __MESSAGE_CALLBACK_H__
#define __MESSAGE_CALLBACK_H__

#include "MpIMsg.h"
#include "JniCallback.h"

class MessageCallback: public MpIMsg {
private:
	JniCallback *messageReceivedJNICallback;
	JniCallback *messageSentJNICallback;

	/**
	 * @brief Ctor
	 * @return NONE
	 */
	MessageCallback(JNIEnv *env);

public:

	/**
	 * @brief Dtor
	 * @return NONE
	 */
	~MessageCallback();
	/**
	 * @brief Message received callback
	 * @param[in] buddy Message sender
	 * @param[in] msg Message
     * @param[in] msgLen Message length
	 * @return NONE
	 */
	void onMsgReceived(std::string const& buddy,  const uint8_t* msg, uint32_t msgLen);
	/**
	 * @brief Message xfer done callback
	 * @param[in] msgId Message id
	 * @param[in] xferStatus SIP xfer status
	 * @return NONE`
	 */
	void onSendMsgDone(void* msgId, mp_status_t xferStatus);

	/**
	 * Singleton Instance 
	 */
	static MessageCallback* getInstance(JNIEnv *env);

	/**
	 * JNI Callback 
	 */
	JniCallback* getMessageReceivedJNICallback();
	JniCallback* getMessageSentJNICallback();
};

#endif /* __MESSAGE_CALLBACK_H__ */
