#include <jni.h>
#include <string>
#include <vector>

#include "MpBaseService.h"
#include "MpSingleton.h"
#include "MpBuddy.h"
#include "MpPresence.h"
#include "MpStatus.h"
#include "MpLogger.h"

extern "C" {

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_addBuddy( JNIEnv* env, jobject thiz,
        jstring buddyUsernameStr) {
        const char *buddyUsername = env->GetStringUTFChars(buddyUsernameStr, 0);

        MP_LOG2("Add buddy for presence ", buddyUsername);

        /* Add buddy and subscribe to presence */
        static MpPresence pres;
        MpService::instance()->getBuddyList()->addPresenceCb(&pres);
        MpBuddy buddy(buddyUsername);
        MpService::instance()->getBuddyList()->addBuddy(buddy);

        return 0;
    }

}