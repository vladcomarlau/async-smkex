#ifndef __ACCOUNT_REG_CALLBACK_H__
#define __ACCOUNT_REG_CALLBACK_H__

#include "MpIRegistration.h"
#include "MpStatus.h"
#include "JniCallback.h"

class AccountRegCallback: public MpIRegistration {
private:

	JniCallback *accountJNICallback;

	/**
	 * @brief Registration state callback
	  * @param[in] status Can be MP_STATUS_ONLINE, MP_STATUS_OFFLINE or MP_STATUS_ERR
	 * @return NONE
	 */
	void registration_state(mp_online_state_t status);

	/**
	 * @brief Ctor
	 * @return NONE
	 */
	AccountRegCallback(JNIEnv *env);

public:

	/**
	 * Singleton Instance 
	 */
	static AccountRegCallback* getInstance(JNIEnv *env);

	/**
	 * @brief Dtor
	 * @return NONE
	 */
	~AccountRegCallback();


	/**
	 * JNI Callback 
	 */
	JniCallback* getCallback();
};

#endif /* __ACCOUNT_REG_CALLBACK_H__ */
