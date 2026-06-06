package org.certsign.smkex.utils.account_status

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter

/**
 * Broadcast receiver of account status updates
 */
class AccountRegStatusBR(
        val listener: IAccountStatusListener?
) : BroadcastReceiver() {

    companion object {
        val STATUS_CHANGED_ACTION = "smkex.status.changed"
        val STATUS_KEY = "smkex.status.data"
    }

    fun register(context: Context?) {
        context?.registerReceiver(this, IntentFilter(STATUS_CHANGED_ACTION))
    }

    fun unregister(context: Context?) {
        context?.unregisterReceiver(this)
    }

    override fun onReceive(context: Context?, intent: Intent?) {
        intent?.getIntExtra(STATUS_KEY, -1)?.let { status ->
            listener?.onAccountStatusChanged(AccountRegStatus.getStatusByType(status))
        }
    }
}