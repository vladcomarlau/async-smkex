package org.certsign.smkex.ui.splash

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.animation.AlphaAnimation
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import kotlinx.android.synthetic.main.activity_splash_screen.*
import org.certsign.smkex.R
import org.certsign.smkex.service.impl.SmkexService
import org.certsign.smkex.ui.dialog.showDialog
import org.certsign.smkex.ui.main.MainActivity
import org.certsign.smkex.utils.getAndroid11StoragePermissionIntent
import org.certsign.smkex.utils.goTo
import org.certsign.smkex.utils.isAndroid11StoragePermissionGranted

class SplashScreenActivity : AppCompatActivity() {

    companion object {
        const val SPLASH_SCREEN_DELAY_MS: Long = 2000L

        const val PERMISSIONS_REQ_CODE: Int = 13581
        const val ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION_REQUEST_CODE = 31241
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_splash_screen)

        // Config views
        configViews()

        // Start Smkex service
        SmkexService.startService(this)

        // Check permissions
        Handler(Looper.getMainLooper()).postDelayed({ checkPermissions() }, SPLASH_SCREEN_DELAY_MS)
    }

    private fun configViews() {
        app_name_text_view.startAnimation(AlphaAnimation(0.2f, 1.0f).apply { duration = 2000L })
    }

    private fun checkPermissions() {
        Log.d(javaClass.simpleName, "Check permissions")
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
            onPermissionsGranted(true)
            return
        }

        // Dangerous permissions
        val permissions = mutableListOf(
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.CALL_PHONE,
                Manifest.permission.READ_CONTACTS,
                Manifest.permission.RECORD_AUDIO,
                Manifest.permission.USE_SIP,
        )
        val notGrantedPermissions = mutableListOf<String>()

        permissions.forEach { permission ->
            if (ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED)
                notGrantedPermissions.add(permission)
        }

        if (notGrantedPermissions.isEmpty()) {
            checkStoragePermissionsForAndroid11()
            return
        }

        Log.d(javaClass.simpleName, "Not granted permissions: $notGrantedPermissions")

        showDialog(
                R.string.permissions_required_label,
                R.string.permissions_details_label,
                { _, _ ->
                    requestPermissions(
                            notGrantedPermissions.toTypedArray(),
                            PERMISSIONS_REQ_CODE
                    )
                }
        )
    }


    private fun checkStoragePermissionsForAndroid11() {
        Log.d(javaClass.simpleName, "Ask for Android11+ Storage Permissions")

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R && !isAndroid11StoragePermissionGranted()) {
            val intent = getAndroid11StoragePermissionIntent(this)
            startActivityForResult(intent, ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION_REQUEST_CODE)
        } else {
            onPermissionsGranted(true)
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        when (requestCode) {
            PERMISSIONS_REQ_CODE -> {
                grantResults.forEach { permissionResult ->
                    if (permissionResult != PackageManager.PERMISSION_GRANTED) {
                        onPermissionsGranted(false)
                        return
                    }
                }
                checkStoragePermissionsForAndroid11()
            }
            else -> {
                // Add more ?
            }
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)

        when (requestCode) {
            ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION_REQUEST_CODE -> onPermissionsGranted(true)
            else -> {
                // Add more ?
            }
        }
    }

    private fun onPermissionsGranted(granted: Boolean) {
        Log.d(javaClass.simpleName, "Permissions granted result: $granted")
        if (granted) goTo(MainActivity::class.java, true)
        else checkPermissions()
    }

}