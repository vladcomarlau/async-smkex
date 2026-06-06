#include <jni.h>
#include <string>

#include "MpBaseService.h"
#include "MpMsg.h"
#include "MpLogger.h"
#include "SmkexKeyExchangedCallback.h"
#include <thread>
#include "SmkexJNI.hpp"
#include "WebSocketsAndroid.hpp"

extern Smkex smkex;
static void task_initiator(JNIEnv* env, std::string b1, std::string b2)
{
	MP_LOG1("In thread INITIATOR MessageService.JNI\n");

	SmkexSessionInfo &session = smkex.initSession(std::string(b1), std::string(b2));
	MP_LOG1("INITIATOR:: Session initiated");

	/* Wait until session is established */
	while(session.getState() != SmkexState::STATEConnected)
	{
		// MP_LOG2_INT("INITIATOR:: Waiting for session to be established. Crt. state: ", session.getState());
		usleep(10 * 1000);
	}
}

extern "C" {

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_sendMessage( JNIEnv* env, jobject thiz,
        jstring destinationIdStr, jstring messagePayloadStr) {
        
        pj_thread_desc a_thread_desc;
        pj_thread_t *a_thread;

        /* Register thread to PJSIP */
        if (!pj_thread_is_registered()) {
            MP_LOG1("Register send-message-thread in PJSIP");
            pj_thread_register("send-message-thread", a_thread_desc, &a_thread);
        }

        const char *destinationId = env->GetStringUTFChars(destinationIdStr, 0);
        const char *messagePayload = env->GetStringUTFChars(messagePayloadStr, 0);

        MP_LOG1("Sending message: First, check SMKEX session state");

        /* Check whether a session exists */
        std::string buddyId1(destinationId) ;
        
        SmkexSessionInfo &session = smkex.getSessionInfo(std::string(buddyId1));

        if (session.getState() == SmkexState::STATEConnected) {
            MP_LOG2("Send new message ", messagePayload);
            MP_LOG2("Message receiver is ", destinationId);

            /* Send message */
            MpBuffer payload((uint8_t*) messagePayload, strlen(messagePayload));
            MpMsgPayload message(destinationId, payload, 1, 5, 1, MP_TYPE_MESSAGE, false);
            MpService::instance()->getAutoResend()->addMessage(message);

            return 0;

        } else { /* Key not exchanged yet */
            MP_LOG1("Sending message: Smkex-key not existing => Start Initiator Thread");

            /* Run initiator */
            std::string buddyId1(destinationId);
            std::string buddyId2 = buddyId1 + "2";
            task_initiator(env, buddyId1, buddyId2);

            /*
            * Sleep for 2 seconds before launching any action, as the destination buddy
            * may still be out of sync with the Smkex key exchange.
            */
            usleep(2 * 1000 * 1000);

            /* Send message */
            MP_LOG2("Sending message Smkex Key-Exchanged finished. Send message to ", destinationId);
            MpBuffer payload((uint8_t*) messagePayload, strlen(messagePayload));
            MpMsgPayload message(destinationId, payload, 1, 5, 1, MP_TYPE_MESSAGE, false);
            MpService::instance()->getAutoResend()->addMessage(message);
            return 0;
        }
    }

}