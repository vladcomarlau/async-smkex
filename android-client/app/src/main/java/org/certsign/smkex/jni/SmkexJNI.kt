package org.certsign.smkex.jni

import android.app.Service
import org.certsign.smkex.jni.callback.*

class SmkexJNI private constructor() {

    companion object {
        private val lock: Any = Any()
        private var instance: SmkexJNI? = null

        // Singleton
        fun getInstance(service: Service?): SmkexJNI? = synchronized(lock) {
            if (service == null) return null

            instance ?: SmkexJNI().also {
                instance = it
            }
        }
    }

    // Load native library
    init {
        System.loadLibrary("smkex")
    }

    // Smkex Init Receiver
    external fun initSmkexWS(clientId: String, buddyId: String): Int

    // Account
    external fun login(clientId: String, sipServer: String, sipPort: Int): Int
    external fun logout(): Int

    // Presence
    external fun addBuddy(buddyId: String): Int

    // Messaging
    external fun sendMessage(buddyId: String, message: String): Int

    // SMKEX Protocol - Receive messages via WS
    external fun init(): Void
    external fun onMsgReceived(buffer: ByteArray?, len: Int, channel: Int)

    // Calls
    external fun placeCall(buddyId: String): Int
    external fun answerCall(): Int
    external fun rejectCall(): Int

    // NDK callbacks subscriptions
    external fun initJNICallbacks()
    external fun removeJNICallbacks()
    external fun subscribeToAccountStatus(listener: IAccountStatusCallbackJNI)
    external fun subscribeToSmkexKeyExchangedStatus(listener: ISmkexKeyExchangedCallbackJNI)
    external fun subscribeToReceivedMessages(listener: IMessageReceivedCallbackJNI)
    external fun subscribeToReceivedCalls(listener: IReceivedCallCallbackJNI)
    external fun subscribeToCallStatus(listener: ICallStatusCallbackJNI)
}