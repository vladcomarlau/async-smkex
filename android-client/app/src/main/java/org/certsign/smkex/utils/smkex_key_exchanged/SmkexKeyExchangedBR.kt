package org.certsign.smkex.utils.smkex_key_exchanged

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter

/**
 * Broadcast receiver of Smkex key-exchanged updates
 */
class SmkexKeyExchangedBR(
        val listener: ISmkexKeyExchangedListener?
) : BroadcastReceiver() {

    companion object {
        val SMKEX_KEY_EXCHANGED_ACTION = "smkex.key.exchanged"
        val STATUS_KEY = "smkex.status.data"
    }

    fun register(context: Context?) {
        context?.registerReceiver(this, IntentFilter(SMKEX_KEY_EXCHANGED_ACTION))
    }

    fun unregister(context: Context?) {
        context?.unregisterReceiver(this)
    }

    override fun onReceive(context: Context?, intent: Intent?) {
        intent?.getIntExtra(STATUS_KEY, -1)?.let { status ->
            listener?.onKeyExchangedStatus(SmkexKeyExchangedStatus.getStatusByType(status))
        }
    }
}