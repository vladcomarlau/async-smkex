package org.certsign.smkex.jni.callback.impl

import android.content.Intent
import android.util.Log
import org.certsign.smkex.jni.callback.IReceivedCallCallbackJNI
import org.certsign.smkex.service.ISmkexService
import org.certsign.smkex.utils.calls.CallsBR

class ReceivedCallCallbackJNI(
        val service: ISmkexService
) : IReceivedCallCallbackJNI {

    /**
     * Triggered on incoming call
     */
    override fun onReceivedCall(from: String) {
        Log.d(javaClass.simpleName, "Received call from $from")
        service.notifyUI(Intent(CallsBR.RECEIVED_CALL_ACTION).apply {
            putExtra(CallsBR.FROM_KEY, from)
        })
    }

}