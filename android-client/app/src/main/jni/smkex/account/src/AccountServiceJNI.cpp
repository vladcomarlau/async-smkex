#include <jni.h>
#include <string>

#include "MpAccSettings.h"
#include "MpRegistration.h"
#include "MpIRegistration.h"
#include "MpMsg.h"
#include "MpLogger.h"
#include "MpCall.h"
#include "AccountRegCallback.h"

static const char ws1[5] = "_ws1";
static const char ws2[5] = "_ws2";

extern "C" {

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_login(JNIEnv* env, jobject thiz, jstring clientIdStr,
        jstring sipServerIpStr, jint sipServerPortJInt) {

        const char *clientId = env->GetStringUTFChars(clientIdStr, 0);
        const char *sipServerIp = env->GetStringUTFChars(sipServerIpStr, 0);
        const int sipServerPort = (int) sipServerPortJInt;

        MP_LOG2("Perform login for user ", clientId);

        MpUserAccount* uc = MpService::instance()->getUserAccount();

        MpAccSettings accSettings(std::string(sipServerIp),
                                   sipServerPort,
                                   std::string(clientId),
                                   10 /* PJSIP log level */,
                                   MP_NETWORK_WIFI,
                                   false,
                                   "/storage/emulated/0/mpvoipapp.crt.pem", /* pjsip client cert */
                                   "/storage/emulated/0/mpvoipapp.key.pem" /* pjsip client prv key */
        );
        mp_status_t status = uc->login(accSettings);

        return status;
    }

    JNIEXPORT int JNICALL
    Java_org_certsign_smkex_jni_SmkexJNI_logout(JNIEnv* env, jobject thiz) {
        MP_LOG1("Perform logout");

        /* Perform logout */
        MpUserAccount* uc = MpService::instance()->getUserAccount();
        mp_status_t status = uc->logout();

        return status;
    }

}