package org.certsign.smkex.utils.smkex_key_exchanged

enum class SmkexKeyExchangedStatus(val type: Int) {
    SUCCESS(0), ERROR(-1);

    companion object {
        fun getStatusByType(type: Int): SmkexKeyExchangedStatus = when (type) {
            0 -> SUCCESS
            1 -> ERROR
            else -> ERROR
        }
    }
}