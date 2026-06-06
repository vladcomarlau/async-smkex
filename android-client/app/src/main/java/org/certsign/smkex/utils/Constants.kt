package org.certsign.smkex.utils

import org.certsign.smkex.BuildConfig

interface Constants {
    companion object {
        // Smkex Server1
        const val SMKEX_SERVER1 = BuildConfig.SMKEX_SERVER1
        const val SMKEX_SERVER1_PORT = BuildConfig.SMKEX_SERVER1_PORT

        // Smkex Server2
        const val SMKEX_SERVER2 = BuildConfig.SMKEX_SERVER2
        const val SMKEX_SERVER2_PORT = BuildConfig.SMKEX_SERVER2_PORT

        // Opensips
        const val SMKEX_OPENSIPS_SERVER = BuildConfig.SMKEX_OPENSIPS_SERVER
        const val SMKEX_OPENSIPS_PORT = BuildConfig.SMKEX_OPENSIPS_PORT

        // Credentials
        const val SMKEX_USERNAME = BuildConfig.SMKEX_USERNAME
        const val SMKEX_PASSWORD = BuildConfig.SMKEX_PASSWORD
    }
}