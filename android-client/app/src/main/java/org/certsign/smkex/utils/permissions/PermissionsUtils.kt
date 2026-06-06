package org.certsign.smkex.utils

import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.os.Environment
import android.provider.Settings

private fun areActivityIntent(context: Context, intent: Intent): Boolean {
    return context.packageManager.queryIntentActivities(intent, PackageManager.MATCH_DEFAULT_ONLY).isNotEmpty()
}

fun getApplicationDetailsIntent(context: Context): Intent {
    val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS)
    intent.data = Uri.parse("package:" + context.packageName)
    return intent
}

fun getAndroid11StoragePermissionIntent(context: Context): Intent {
    var intent: Intent? = null
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) { // is android 11+
        intent = Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION)
        intent.data = Uri.parse("package:" + context.packageName)
    }
    if (intent == null || !areActivityIntent(context, intent)) {
        intent = getApplicationDetailsIntent(context)
    }
    return intent
}

fun isAndroid11StoragePermissionGranted() = Environment.isExternalStorageManager()