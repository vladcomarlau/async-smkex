#ifndef __DEFAULT_JNI_CALLBACK_H__
#define __DEFAULT_JNI_CALLBACK_H__

#include <string.h>
#include <jni.h>
#include <vector>

#include "ObserverChain.h"
#include "MpBaseService.h"
#include "MpLogger.h"
#include "MpStatus.h"

class JniCallback {

public:

	/**
	 * Constructor
	 */
	JniCallback();

	/**
	 * Destructor 
	 */
	~JniCallback();

	/**
	 * Init JNI callback 
	 */
	void init(JNIEnv *env);

	/**
	 * Remove all listeners
	 */
	void dismissListeners();

	/**
	 * Add listener
	 */
	void addListener(jweak store_Wlistener, jmethodID store_method);

	/**
	 * Notify listener with structure{String, ByteArray, BytesLength} message 
	 */
	void notifyListenersWithGenericMessage(const char* str, const uint8_t* bytes, uint32_t bytesLen);

	/**
	 * Notify listene with a char* message 
	 */
	void notifyListenersWithMessage(const char* str);

	/**
	 * Notify listener with integer message
	 */
	void notifyListenersWithValue(jint messageVal_);

	/* Java VM pointer */
	static JavaVM *jvm;

	/* Saved listeners */
	std::vector<ObserverChain *> store_Wlistener_vector;
};

#endif /* __DEFAULT_JNI_CALLBACK_H__ */