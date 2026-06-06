package org.certsign.smkex.utils.messages

interface IMessageReceivedListener {
    fun onMessageReceived(messageData: MessageReceivedData)
}