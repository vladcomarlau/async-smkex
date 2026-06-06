package org.certsign.smkex.ui.main.home

interface IHome {
    fun onError(message: String)

    fun onLoginSucceeded()

    fun onLogoutSucceeded()

    fun onBuddyPresenceAdded()
}