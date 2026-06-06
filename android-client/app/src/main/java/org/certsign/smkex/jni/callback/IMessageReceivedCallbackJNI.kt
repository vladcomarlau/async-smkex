package org.certsign.smkex.jni.callback

interface IMessageReceivedCallbackJNI {
    fun onMessageReceived(senderURI: String?, messageContent: ByteArray?, messageLen: Int)
}