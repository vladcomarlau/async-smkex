package org.certsign.smkex.networking

import android.annotation.SuppressLint
import android.content.Context
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import android.util.Log
import okhttp3.Credentials
import okhttp3.Interceptor
import okhttp3.OkHttpClient
import okhttp3.Response
import org.certsign.smkex.service.impl.SmkexService
import org.certsign.smkex.utils.Constants.Companion.SMKEX_PASSWORD
import org.certsign.smkex.utils.Constants.Companion.SMKEX_SERVER1
import org.certsign.smkex.utils.Constants.Companion.SMKEX_SERVER1_PORT
import org.certsign.smkex.utils.Constants.Companion.SMKEX_SERVER2
import org.certsign.smkex.utils.Constants.Companion.SMKEX_SERVER2_PORT
import org.certsign.smkex.utils.Constants.Companion.SMKEX_USERNAME
import java.security.SecureRandom
import java.security.cert.X509Certificate
import javax.net.ssl.SSLContext
import javax.net.ssl.SSLSocketFactory
import javax.net.ssl.TrustManager
import javax.net.ssl.X509TrustManager
import javax.security.cert.CertificateException

class NetworkingUtils {

    companion object {

        // SMKEX Channels IDs
        const val SMKEX_CHANNEL1 = 0
        const val SMKEX_CHANNEL2 = 1

        // Networking Channels IDs
        const val WIFI_CHANNEL = 0
        const val CELLULAR_CHANNEL = 1

        // WebSocket endpoints
        val SMKEX_WS_ADDR1 = "https://$SMKEX_SERVER1:$SMKEX_SERVER1_PORT/public/smkex-websocket"
        val SMKEX_WS_ADDR2 = "https://$SMKEX_SERVER2:$SMKEX_SERVER2_PORT/public/smkex-websocket"

        // HTTP based endpoints
        val SMKEX_HTTP_ADDR1 = "https://$SMKEX_SERVER1:$SMKEX_SERVER1_PORT/"
        val SMKEX_HTTP_ADDR2 = "https://$SMKEX_SERVER2:$SMKEX_SERVER2_PORT/"


        fun displayAvailableNetworkProviders(context: Context?) {
            try {
                val connManager = context?.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager

                connManager.getNetworkCapabilities(connManager.activeNetwork)?.let {
                    when {
                        it.hasTransport(NetworkCapabilities.TRANSPORT_WIFI) -> {
                            Log.d(SmkexNetworkingAPI.TAG, "WIFI is available")
                        }
                        it.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) -> {
                            Log.d(SmkexNetworkingAPI.TAG, "CELLULAR is available")
                        }
                        it.hasTransport(NetworkCapabilities.TRANSPORT_VPN) -> {
                            Log.d(SmkexNetworkingAPI.TAG, "VPN is available")
                        }
                        else -> {
                            Log.e(SmkexNetworkingAPI.TAG, "No internet provider available!")
                        }
                    }
                }
            } catch (e: Exception) {
                Log.e(SmkexNetworkingAPI.TAG, "Failed to retrieve network information", e)
            }
        }

        // Basic Auth Interceptor used for SMKEX servers
        class BasicAuthInterceptor(user: String, password: String) : Interceptor {
            private val credentials: String = Credentials.basic(user, password)

            override fun intercept(chain: Interceptor.Chain): Response {
                val request = chain.request()
                val authenticatedRequest = request.newBuilder()
                    .header("Authorization", credentials).build()
                return chain.proceed(authenticatedRequest)
            }
        }

        // Return UNSAFE http client
        fun getUnsafeOkHttpClient(): OkHttpClient.Builder =
            try {
                // Create a trust manager that does not validate certificate chains
                val trustAllCerts: Array<TrustManager> = arrayOf(
                    @SuppressLint("CustomX509TrustManager")
                    object : X509TrustManager {
                        @Throws(CertificateException::class)
                        override fun checkClientTrusted(
                            chain: Array<X509Certificate?>?,
                            authType: String?
                        ) = Unit

                        @Throws(CertificateException::class)
                        override fun checkServerTrusted(
                            chain: Array<X509Certificate?>?,
                            authType: String?
                        ) = Unit

                        override fun getAcceptedIssuers(): Array<X509Certificate> = arrayOf()
                    }
                )

                // Install the all-trusting trust manager
                val sslContext: SSLContext = SSLContext.getInstance("SSL")
                sslContext.init(null, trustAllCerts, SecureRandom())

                // Create an ssl socket factory with our all-trusting manager
                val sslSocketFactory: SSLSocketFactory = sslContext.socketFactory
                val builder = OkHttpClient.Builder().addInterceptor(
                    BasicAuthInterceptor(SMKEX_USERNAME, SMKEX_PASSWORD)
                )

                builder.sslSocketFactory(sslSocketFactory, trustAllCerts[0] as X509TrustManager)
                builder.hostnameVerifier { _, _ -> true }
                builder
            } catch (e: Exception) {
                throw RuntimeException(e)
            }
    }


}