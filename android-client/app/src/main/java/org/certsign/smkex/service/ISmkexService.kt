package org.certsign.smkex.service

import android.content.Intent
import org.certsign.smkex.utils.account_status.AccountRegStatus
import org.certsign.smkex.utils.smkex_key_exchanged.SmkexKeyExchangedStatus

interface ISmkexService {
    fun notifyUI(intent: Intent)
}