package org.certsign.smkex.jni.callback.impl

import android.content.Intent
import android.util.Log
import org.certsign.smkex.jni.callback.ISmkexKeyExchangedCallbackJNI
import org.certsign.smkex.service.ISmkexService
import org.certsign.smkex.utils.smkex_key_exchanged.SmkexKeyExchangedBR

class SmkexKeyExchangedCallbackJNI(
        val service: ISmkexService
) : ISmkexKeyExchangedCallbackJNI {

    /**
     * Triggered on Smkex key exchanged
     */
    override fun onKeyExchanged(status: Int) {
        Log.d(javaClass.simpleName, "Smkex Key-Exchanged status : $status")

        service.notifyUI(Intent(SmkexKeyExchangedBR.SMKEX_KEY_EXCHANGED_ACTION).apply {
            putExtra(SmkexKeyExchangedBR.STATUS_KEY, status)
        })
    }
}