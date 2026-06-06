package org.certsign.smkex.jni.callback

interface ICallStatusCallbackJNI {
    fun onCallStatusReceived(status: Int)
}