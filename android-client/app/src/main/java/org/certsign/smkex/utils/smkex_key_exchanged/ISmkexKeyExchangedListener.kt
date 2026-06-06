package org.certsign.smkex.utils.smkex_key_exchanged

interface ISmkexKeyExchangedListener {
    fun onKeyExchangedStatus(status: SmkexKeyExchangedStatus)
}