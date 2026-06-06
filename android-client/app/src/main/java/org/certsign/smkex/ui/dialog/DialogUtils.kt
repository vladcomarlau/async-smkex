package org.certsign.smkex.ui.dialog

import android.content.Context
import android.content.DialogInterface
import androidx.appcompat.app.AlertDialog

fun Context.showDialog(
        titleRes: Int,
        messageRes: Int,
        btn1: DialogInterface.OnClickListener? = null,
        btn2: DialogInterface.OnClickListener? = null
) = showDialog(getString(titleRes), getString(messageRes), btn1, btn2)

fun Context.showDialog(
        titleRes: String,
        messageRes: String,
        btn1: DialogInterface.OnClickListener? = null,
        btn2: DialogInterface.OnClickListener? = null
) = try {
    AlertDialog.Builder(this).apply {
        setTitle(titleRes)
        setMessage(messageRes)
        if (btn1 != null) setPositiveButton(android.R.string.ok, btn1)
        if (btn2 != null) setNegativeButton(android.R.string.cancel, btn2)
    }.show()
} catch (e: Exception) {
    e.printStackTrace()
}
