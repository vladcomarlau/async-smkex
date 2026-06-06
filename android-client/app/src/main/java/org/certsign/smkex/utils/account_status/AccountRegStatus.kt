package org.certsign.smkex.utils.account_status

enum class AccountRegStatus(val type: Int) {
    ONLINE(0), OFFLINE(1), ERROR(-1);

    companion object {
        fun getStatusByType(type: Int): AccountRegStatus = when (type) {
            0 -> ONLINE
            1 -> OFFLINE
            else -> ERROR
        }
    }
}