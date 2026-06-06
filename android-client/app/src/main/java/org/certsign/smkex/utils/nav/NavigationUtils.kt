package org.certsign.smkex.utils

import android.app.Activity
import android.content.Intent

fun Activity.goTo(destination: Class<*>?, finishedCurrent: Boolean = false) {
    startActivity(Intent(this, destination)).also {
        if (finishedCurrent) finish()
    }
}

