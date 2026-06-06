package org.certsign.smkex.ui.main.home

import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.core.widget.addTextChangedListener
import androidx.fragment.app.Fragment
import com.google.android.material.snackbar.Snackbar
import kotlinx.android.synthetic.main.fragment_home.*
import org.certsign.smkex.R
import org.certsign.smkex.service.impl.SmkexService
import org.certsign.smkex.ui.dialog.showDialog
import org.certsign.smkex.utils.account_status.AccountRegStatus
import org.certsign.smkex.utils.account_status.AccountRegStatusBR
import org.certsign.smkex.utils.account_status.IAccountStatusListener
import org.certsign.smkex.utils.calls.CallsBR
import org.certsign.smkex.utils.calls.ICallsListener
import org.certsign.smkex.utils.messages.IMessageReceivedListener
import org.certsign.smkex.utils.messages.MessageReceivedBR
import org.certsign.smkex.utils.messages.MessageReceivedData
import org.certsign.smkex.utils.os.hideKeyboard
import org.certsign.smkex.utils.smkex_key_exchanged.ISmkexKeyExchangedListener
import org.certsign.smkex.utils.smkex_key_exchanged.SmkexKeyExchangedBR
import org.certsign.smkex.utils.smkex_key_exchanged.SmkexKeyExchangedStatus

class HomeFragment : Fragment(),
        IHome, // Main features (used by controller callbacks)
        IAccountStatusListener, // Account Registration Updates
        ISmkexKeyExchangedListener, // Smkex Key-Exchanged Updates
        IMessageReceivedListener, // Incoming SIP messages Updates
        ICallsListener // Incoming Call or Call-Response Updates
{
    private lateinit var controller: HomeController

    private val accountStatusBR = AccountRegStatusBR(this)
    private val smkexKeyExchangedBR = SmkexKeyExchangedBR(this)
    private val messageReceivedBR = MessageReceivedBR(this)
    private val callsBR = CallsBR(this)

    override fun onResume() {
        Log.d(javaClass.simpleName, "onResume")
        super.onResume()

        accountStatusBR.register(activity)
        messageReceivedBR.register(activity)
        callsBR.register(activity)
        smkexKeyExchangedBR.register(activity)
    }

    override fun onPause() {
        Log.d(javaClass.simpleName, "onPause")
        accountStatusBR.unregister(activity)
        messageReceivedBR.unregister(activity)
        callsBR.unregister(activity)
        smkexKeyExchangedBR.unregister(activity)

        super.onPause()
    }

    override fun onCreateView(
            inflater: LayoutInflater,
            container: ViewGroup?,
            savedInstanceState: Bundle?
    ): View? = inflater.inflate(R.layout.fragment_home, container, false)

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        // Init controller
        controller = HomeController(this)

        // Setup UI
        configViews()
    }

    private fun configViews() {
        // Account
        login_button.setOnClickListener {
            hideKeyboard()
            val clientId = user_serial_edit_text.text.toString()
            controller.login(clientId)
        }
        logout_button.setOnClickListener { controller.logout() }

        // Presence
        buddy_username_edit_text.addTextChangedListener { add_buddy_button.isEnabled = true }
        add_buddy_button.setOnClickListener {
            hideKeyboard()
            val clientId = user_serial_edit_text.text.toString()
            val buddyId = buddy_username_edit_text.text.toString()
            controller.subscribeToBuddyPresence(clientId, buddyId)
        }

        // Messages
        send_message_button.setOnClickListener {
            hideKeyboard()
            val buddyId = buddy_username_edit_text.text.toString()
            val message = message_edit_text.text.toString()
            controller.sendMessage(buddyId, message)
        }

        // Calls
        launch_call_button.setOnClickListener {
            hideKeyboard()
            val buddyId = buddy_username_edit_text.text.toString()
            controller.placeCall(buddyId)
        }

    }

    override fun onError(message: String) = Toast.makeText(activity, message, Toast.LENGTH_SHORT).show()

    override fun onLoginSucceeded() {
        user_serial_edit_text.isEnabled = false
        logout_button.visibility = View.VISIBLE
        login_button.visibility = View.GONE
    }

    override fun onLogoutSucceeded() {
        user_serial_edit_text.isEnabled = true
        logout_button.visibility = View.GONE
        login_button.visibility = View.VISIBLE
    }

    override fun onBuddyPresenceAdded() {
        add_buddy_button.isEnabled = false
    }

    override fun onAccountStatusChanged(status: AccountRegStatus) {
        Log.d(javaClass.simpleName, "Account status received: ${status.name}")
        view?.let { Snackbar.make(it, "You are now ${status.name}", Snackbar.LENGTH_SHORT).show() }
    }

    override fun onKeyExchangedStatus(status: SmkexKeyExchangedStatus) {
        Log.d(javaClass.simpleName, "Smkex Key-Exchanged status received: ${status.name}")
        view?.let { Snackbar.make(it, "Smkex Key-Exchanged finished with status: ${status.name}", Snackbar.LENGTH_SHORT).show() }
    }

    override fun onMessageReceived(messageData: MessageReceivedData) {
        val title = "Received message from " + messageData.senderUri
        val message = if (messageData.messageContent != null)
            String(messageData.messageContent, 0, messageData.messageLen!!)
        else "N/A"
        activity?.showDialog(title, message)
    }

    override fun onCallReceived(from: String?) {
        activity?.showDialog(
                "You received a call",
                "The caller is $from",
                { _, _ -> SmkexService.answerCall() },
                { _, _ -> SmkexService.rejectCall() }
        )
    }

    override fun onCallResponseReceived(status: Int) {
        Log.d(javaClass.simpleName, "Received call status: $status")
        view?.let {
            Snackbar.make(it, "Call Status: $status", Snackbar.LENGTH_SHORT).show()
        }
    }
}