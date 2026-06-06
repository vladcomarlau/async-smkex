#include <jni.h>
#include <string>

#include "MpBaseService.h"
#include "MpSingleton.h"
#include "MpBuddy.h"
#include "MpStatus.h"
#include "MpLogger.h"
#include "MpCall.h"
#include "Smkex.h"
#include "SmkexSessionInfo.h"
#include "SmkexKeyExchangedCallback.h"

#include <thread>

extern Smkex smkex;
static void task_initiator(JNIEnv* env, std::string b1, std::string b2)
{
	MP_LOG1("In thread INITIATOR CallService.JNI\n");

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
    Java_org_certsign_smkex_jni_SmkexJNI_placeCall( JNIEnv* env, jobject thiz, jstring destinationIdStr) {

        pj_thread_desc a_thread_desc;
        pj_thread_t *a_thread;

        /* Register thread to PJSIP */
        if (!pj_thread_is_registered()) {
            MP_LOG1("Register place-call-thread in PJSIP");
            pj_thread_register("place-call-thread", a_thread_desc, &a_thread);
        }
        
        const char *destinationId = env->GetStringUTFChars(destinationIdStr, 0);
        MP_LOG2("Place call to ", destinationId);

        /* Check whether a session exists */
        std::string buddyId1(destinationId);
        SmkexSessionInfo &session = smkex.getSessionInfo(std::string(buddyId1));

        if (session.getState() == SmkexState::STATEConnected) {
            /* Call buddy */
            MpService::instance()->getCallManager()->callBuddy(destinationId);
            return 0;

        } else { /* Key not exchanged yet */
            MP_LOG1("Place call: Smkex-key not existing => Start Initiator Thread");

            /* Run Initiator */
            std::string buddyId1(destinationId);
            std::string buddyId2 = buddyId1 + "2";
            task_initiator(env, buddyId1, buddyId2);

            /*
            * Sleep for 2 seconds before launching any action, as the destination buddy
            * may still be out of sync with the Smkex key exchange.
            */
            usleep(2 * 1000 * 1000);

            /* Call buddy */
            MP_LOG2("Place call: Smkex Key-Exchanged finished. Place call to ", destinationId);
            MpService::instance()->getCallManager()->callBuddy(destinationId);
            return 0;
        }
    }

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_answerCall( JNIEnv* env, jobject thiz) {
        MP_LOG1("Answer call");
        MpService::instance()->getCallManager()->answerCall(MP_ANSWER_CALL);
        return 0;
    }

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_rejectCall( JNIEnv* env, jobject thiz) {
        MP_LOG1("Reject call");
        MpService::instance()->getCallManager()->answerCall(MP_REJECT_CALL);
        return 0;
    }
}