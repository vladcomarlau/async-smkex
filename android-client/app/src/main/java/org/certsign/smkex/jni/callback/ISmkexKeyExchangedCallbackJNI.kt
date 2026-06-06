package org.certsign.smkex.jni.callback

interface ISmkexKeyExchangedCallbackJNI {
    fun onKeyExchanged(status: Int)
}