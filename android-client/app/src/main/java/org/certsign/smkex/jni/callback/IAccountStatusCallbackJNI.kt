package org.certsign.smkex.jni.callback

interface IAccountStatusCallbackJNI {
    fun onAccountStatusReceived(status: Int)
}