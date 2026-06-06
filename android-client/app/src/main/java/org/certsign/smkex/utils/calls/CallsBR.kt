package org.certsign.smkex.utils.calls

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter

class CallsBR(
        val listener: ICallsListener?
) : BroadcastReceiver() {

    companion object {
        val RECEIVED_CALL_ACTION = "smkex.received.call"
        val CALL_STATUS_ACTION = "smkex.call.status"
        val FROM_KEY = "smkex.from.data"
        val CALL_STATUS_KEY = "smkex.call.data"
    }

    fun register(context: Context?) {
        context?.registerReceiver(this, IntentFilter().apply {
            addAction(RECEIVED_CALL_ACTION)
            addAction(CALL_STATUS_ACTION)
        })
    }

    fun unregister(context: Context?) {
        context?.unregisterReceiver(this)
    }

    override fun onReceive(context: Context?, intent: Intent?) {
        when (intent?.action) {
            RECEIVED_CALL_ACTION -> {
                val from = intent.getStringExtra(FROM_KEY) ?: ""
                listener?.onCallReceived(from)
            }
            CALL_STATUS_ACTION -> {
                val status = intent.getIntExtra(CALL_STATUS_KEY, -1)
                listener?.onCallResponseReceived(status)
            }
        }
    }
}