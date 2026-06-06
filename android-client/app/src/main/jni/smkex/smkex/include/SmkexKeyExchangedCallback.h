#ifndef __SMKEX_KEY_EXCHANGED_CALLBACK_H__
#define __SMKEX_KEY_EXCHANGED_CALLBACK_H__

#include "JniCallback.h"

class SmkexKeyExchangedCallback {
private:

	JniCallback *smkexJNICallback;

	/**
	 * @brief Ctor
	 * @return NONE
	 */
	SmkexKeyExchangedCallback(JNIEnv *env);

public:

	/**
	 * Singleton Instance 
	 */
	static SmkexKeyExchangedCallback* getInstance(JNIEnv *env);

	/**
	 * @brief Dtor
	 * @return NONE
	 */
	~SmkexKeyExchangedCallback();


	/**
	 * JNI Callback 
	 */
	JniCallback* getCallback();


	/**
	 * Notify with Smkex Key-Exchanged Failure 
	 * 
	 * @status Failure Status
	 */
	void onKeyExchangedFailed(int status);

	/**
	 * Notify with Smkex Key-Exchanged Success 
	 */
	void onKeyExchangedSucceeded();

};

#endif /* __SMKEX_KEY_EXCHANGED_CALLBACK_H__ */
