package org.certsign.smkex.ui.main.home

import android.util.Log
import org.certsign.smkex.service.impl.SmkexService
import org.certsign.smkex.utils.Constants

class HomeController(
    val listener: IHome?
) {
    fun login(clientId: String?) {
        if (clientId.isNullOrEmpty()) {
            listener?.onError("Please fill the username")
            return
        }

        val status = SmkexService.login(
            clientId,
            Constants.SMKEX_OPENSIPS_SERVER,
            Constants.SMKEX_OPENSIPS_PORT
        )
        Log.d(javaClass.simpleName, "Login status: $status")

        if (status == 0) {
            listener?.onLoginSucceeded()
        }
    }

    fun logout() {
        val status = SmkexService.logout()
        Log.d(javaClass.simpleName, "Logout status: $status")

        if (status == 0)
            listener?.onLogoutSucceeded()
    }

    fun subscribeToBuddyPresence(clientId: String?, buddyId: String?) {
        if (clientId.isNullOrEmpty() || buddyId.isNullOrEmpty()) {
            listener?.onError("Please fill the username and buddyName")
            return
        }

        Log.d(javaClass.simpleName, "Init SMKEX Websockets")
        SmkexService.initSmkexWS(clientId, buddyId)

        Log.d(javaClass.simpleName, "Subscribe to buddy presence status")
        val status = SmkexService.addBuddy(buddyId)

        Log.d(javaClass.simpleName, "Add-buddy status: $status")
        listener?.onBuddyPresenceAdded()
    }

    fun sendMessage(buddyId: String?, message: String?) {
        if (buddyId.isNullOrEmpty() || message.isNullOrEmpty()) {
            listener?.onError("Please fill the buddy ID and message content")
            return
        }

        SmkexService.sendMessage(buddyId, message)
    }

    fun placeCall(buddyId: String?) {
        if (buddyId.isNullOrEmpty()) {
            listener?.onError("Please fill the buddy ID")
            return
        }

        SmkexService.placeCall(buddyId)
    }

}