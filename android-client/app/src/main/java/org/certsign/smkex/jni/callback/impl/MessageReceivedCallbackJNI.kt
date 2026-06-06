package org.certsign.smkex.jni.callback.impl

import android.content.Intent
import android.util.Log
import org.certsign.smkex.jni.callback.IMessageReceivedCallbackJNI
import org.certsign.smkex.service.ISmkexService
import org.certsign.smkex.utils.messages.MessageReceivedBR

class MessageReceivedCallbackJNI(
        val service: ISmkexService
) : IMessageReceivedCallbackJNI {

    /**
     * Triggered on incoming PJSIP messages
     */
    override fun onMessageReceived(senderURI: String?, messageContent: ByteArray?, messageLen: Int) {
        Log.d(javaClass.simpleName, "Received message from $senderURI")

        service.notifyUI(Intent(MessageReceivedBR.MESSAGE_RECEIVED_ACTION).apply {
            putExtra(MessageReceivedBR.SENDER_URI_KEY, senderURI)
            putExtra(MessageReceivedBR.MESSAGE_CONTENT_KEY, messageContent)
            putExtra(MessageReceivedBR.MESSAGE_LEN_KEY, messageLen)
        })
    }
}