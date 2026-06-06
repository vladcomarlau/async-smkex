package org.certsign.smkex.jni.callback.impl

import android.content.Intent
import android.util.Log
import org.certsign.smkex.jni.callback.IAccountStatusCallbackJNI
import org.certsign.smkex.service.ISmkexService
import org.certsign.smkex.utils.account_status.AccountRegStatusBR

class AccountStatusCallbackJNI(
        val service: ISmkexService
) : IAccountStatusCallbackJNI {

    /**
     * Triggered on user status updates
     */
    override fun onAccountStatusReceived(status: Int) {
        Log.d(javaClass.simpleName, "Account status : $status")

        service.notifyUI(Intent(AccountRegStatusBR.STATUS_CHANGED_ACTION).apply {
            putExtra(AccountRegStatusBR.STATUS_KEY, status)
        })
    }
}