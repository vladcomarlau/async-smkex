package org.certsign.smkex.utils.messages

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter

class MessageReceivedBR(
        val listener: IMessageReceivedListener?
) : BroadcastReceiver() {

    companion object {
        val MESSAGE_RECEIVED_ACTION = "smkex.message.received"
        val SENDER_URI_KEY = "smkex.sender.uri"
        val MESSAGE_CONTENT_KEY = "smkex.message.content"
        val MESSAGE_LEN_KEY = "smkex.message.length"
    }

    fun register(context: Context?) {
        context?.registerReceiver(this, IntentFilter(MESSAGE_RECEIVED_ACTION))
    }

    fun unregister(context: Context?) {
        context?.unregisterReceiver(this)
    }

    override fun onReceive(context: Context?, intent: Intent?) {
        intent?.let {
            val senderUri = it.getStringExtra(SENDER_URI_KEY)
            val messageContent = it.getByteArrayExtra(MESSAGE_CONTENT_KEY)
            val messageLength = it.getIntExtra(MESSAGE_LEN_KEY, -1)

            listener?.onMessageReceived(MessageReceivedData(
                    senderUri,
                    messageContent,
                    messageLength
            ))
        }
    }

}