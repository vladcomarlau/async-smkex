package org.certsign.smkex.utils.account_status

interface IAccountStatusListener {
    fun onAccountStatusChanged(status: AccountRegStatus)
}