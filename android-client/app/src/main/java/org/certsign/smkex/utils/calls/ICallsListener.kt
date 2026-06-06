package org.certsign.smkex.utils.calls

interface ICallsListener {
    fun onCallReceived(from: String?)
    fun onCallResponseReceived(status: Int)
}