package org.certsign.smkex.jni.callback.impl

import android.content.Intent
import android.util.Log
import org.certsign.smkex.jni.callback.ICallStatusCallbackJNI
import org.certsign.smkex.service.ISmkexService
import org.certsign.smkex.utils.calls.CallsBR

class CallStatusCallbackJNI(
        val service: ISmkexService
) : ICallStatusCallbackJNI {

    /**
     * Triggered on received call status
     */
    override fun onCallStatusReceived(status: Int) {
        Log.d(javaClass.simpleName, "Received call status $status")
        service.notifyUI(Intent(CallsBR.CALL_STATUS_ACTION).apply {
            putExtra(CallsBR.CALL_STATUS_KEY, status)
        })
    }
}