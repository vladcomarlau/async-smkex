package org.certsign.smkex.service.impl

import android.app.Service
import android.content.Context
import android.content.Intent
import android.os.IBinder
import android.util.Log
import org.certsign.smkex.jni.SmkexJNI
import org.certsign.smkex.jni.callback.*
import org.certsign.smkex.jni.callback.impl.*
import org.certsign.smkex.networking.SmkexNetworkingAPI
import org.certsign.smkex.service.ISmkexService

class SmkexService : Service(), ISmkexService {

    override fun onCreate() {
        super.onCreate()
        Log.d(javaClass.simpleName, "Smkex service created")

        instance = this

        // Init JNI after service creation
        jniService = SmkexJNI.getInstance(this)!!
        init()
        initJNICallbacks()

        // Init networking data
        SmkexNetworkingAPI.init()

        // Add JNI callbacks
        subscribeToAccountStatus(AccountStatusCallbackJNI(this))
        subscribeToSmkexKeyExchangedStatus(SmkexKeyExchangedCallbackJNI(this))
        subscribeToReceivedMessages(MessageReceivedCallbackJNI(this))
        subscribeToReceivedCalls(ReceivedCallCallbackJNI(this))
        subscribeToCallStatus(CallStatusCallbackJNI(this))
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        Log.d(javaClass.simpleName, "Smkex service started")
        return START_STICKY
    }

    override fun onBind(intent: Intent?): IBinder? = null

    override fun onDestroy() {
        Log.d(javaClass.simpleName, "Smkex service stopped")
        removeJNICallbacks()
        super.onDestroy()
    }

    override fun notifyUI(intent: Intent) = sendBroadcast(intent)

    companion object {
        private lateinit var jniService: SmkexJNI

        private var instance: SmkexService? = null

        fun getInstance(): SmkexService? = instance

        // Service handler
        fun startService(context: Context) =
            context.startService(Intent(context, SmkexService::class.java))

        fun stopService(context: Context) =
            context.stopService(Intent(context, SmkexService::class.java))

        // Smkex Init Receiver
        fun init() = jniService.init()
        fun initSmkexWS(clientId: String, buddyId: String) =
            jniService.initSmkexWS(clientId, buddyId)

        // Account
        fun login(clientId: String, sipServer: String, sipPort: Int): Int = jniService.login(
            clientId,
            sipServer,
            sipPort
        )

        fun logout(): Int = jniService.logout()

        // Presence
        fun addBuddy(buddyId: String): Int = jniService.addBuddy(buddyId)

        // Messaging
        fun sendMessage(buddyId: String, message: String) =
            Thread { jniService.sendMessage(buddyId, message) }.start()

        fun onMsgReceived(buffer: ByteArray?, len: Int, channel: Int) =
            jniService.onMsgReceived(buffer, len, channel)

        // Calls
        fun placeCall(buddyId: String) = Thread { jniService.placeCall(buddyId) }.start()
        fun answerCall(): Int = jniService.answerCall()
        fun rejectCall(): Int = jniService.rejectCall()

        // NDK Callbacks
        fun initJNICallbacks() = jniService.initJNICallbacks()
        fun removeJNICallbacks() = jniService.removeJNICallbacks()
        fun subscribeToAccountStatus(listener: IAccountStatusCallbackJNI) =
            jniService.subscribeToAccountStatus(listener)

        fun subscribeToSmkexKeyExchangedStatus(listener: ISmkexKeyExchangedCallbackJNI) =
            jniService.subscribeToSmkexKeyExchangedStatus(listener)

        fun subscribeToReceivedMessages(lister: IMessageReceivedCallbackJNI) =
            jniService.subscribeToReceivedMessages(lister)

        fun subscribeToReceivedCalls(listener: IReceivedCallCallbackJNI) =
            jniService.subscribeToReceivedCalls(listener)

        fun subscribeToCallStatus(listener: ICallStatusCallbackJNI) =
            jniService.subscribeToCallStatus(listener)
    }
}