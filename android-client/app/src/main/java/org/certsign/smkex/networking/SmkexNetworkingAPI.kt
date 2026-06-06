package org.certsign.smkex.networking

import android.content.Context.CONNECTIVITY_SERVICE
import android.net.ConnectivityManager
import android.net.ConnectivityManager.NetworkCallback
import android.net.Network
import android.net.NetworkCapabilities
import android.net.NetworkRequest
import android.os.Build
import android.util.Log
import androidx.annotation.RequiresApi
import okhttp3.*
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.RequestBody.Companion.toRequestBody
import org.certsign.smkex.networking.NetworkingUtils.Companion.CELLULAR_CHANNEL
import org.certsign.smkex.networking.NetworkingUtils.Companion.SMKEX_CHANNEL1
import org.certsign.smkex.networking.NetworkingUtils.Companion.SMKEX_CHANNEL2
import org.certsign.smkex.networking.NetworkingUtils.Companion.SMKEX_HTTP_ADDR1
import org.certsign.smkex.networking.NetworkingUtils.Companion.SMKEX_HTTP_ADDR2
import org.certsign.smkex.networking.NetworkingUtils.Companion.SMKEX_WS_ADDR1
import org.certsign.smkex.networking.NetworkingUtils.Companion.SMKEX_WS_ADDR2
import org.certsign.smkex.networking.NetworkingUtils.Companion.WIFI_CHANNEL
import org.certsign.smkex.networking.NetworkingUtils.Companion.displayAvailableNetworkProviders
import org.certsign.smkex.networking.NetworkingUtils.Companion.getUnsafeOkHttpClient
import org.certsign.smkex.service.impl.SmkexService
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory


class SmkexNetworkingAPI {

    companion object {

        const val TAG = "SmkexNetworkingAPI"

        // Websocket clients
        private var webSocketWifiClient: OkHttpClient? = null
        private var webSocketCellularClient: OkHttpClient? = null

        // Client ID
        private var clientID: String? = null

        // Channel Mapping: SMKEX_CHANNEL -> NETWORK_CHANNEL
        val channelMapping = mapOf(
            Pair(SMKEX_CHANNEL1, WIFI_CHANNEL),
            Pair(SMKEX_CHANNEL2, WIFI_CHANNEL) // TODO: Change this
        )

        // WS Server Endpoint Mapping
        val wsServersMappingPerChannel = mapOf(
            Pair(SMKEX_CHANNEL1, SMKEX_WS_ADDR1),
            Pair(SMKEX_CHANNEL2, SMKEX_WS_ADDR2)
        )

        // HTTP Server Endpoint Mapping
        val httpServersMappingPerChannel = mapOf(
            Pair(SMKEX_CHANNEL1, SMKEX_HTTP_ADDR1),
            Pair(SMKEX_CHANNEL2, SMKEX_HTTP_ADDR2)
        )

        fun init() {
            displayAvailableNetworkProviders(
                SmkexService.getInstance()?.applicationContext
            )

            // Create web-socket and http clients
            webSocketWifiClient = OkHttpClient()
            webSocketCellularClient = OkHttpClient()
        }
    }

    // Called from JNI
    @RequiresApi(Build.VERSION_CODES.M)
    fun initWebSockets(clientId: String, channel: Int) {
        val transportChannel = channelMapping[channel]
        val wsServerEndpoint = wsServersMappingPerChannel[channel]

        Log.d(
            TAG,
            "Init web socket for client $clientId using smkex-channel $channel and network-channel $transportChannel"
        )

        when (transportChannel) {
            WIFI_CHANNEL -> initWebSocketConnectionOnWifi(clientId, channel, wsServerEndpoint)
            CELLULAR_CHANNEL -> initWebSocketConnectionOnCellular(
                clientId,
                channel,
                wsServerEndpoint
            )
            else -> Log.e(TAG, "Invalid channel id: $channel")
        }
    }

    private fun initWebSocketConnectionOnWifi(
        clientId: String,
        channel: Int,
        wsServerEndpoint: String?
    ) {
        clientID = clientId

        // Config WS connection for WiFi
        val wsConnectionUrl = "$wsServerEndpoint/$clientId"
        val request = Request.Builder().url(wsConnectionUrl).build()
        val wifiNetworkRequest = NetworkRequest.Builder()
            .addTransportType(NetworkCapabilities.TRANSPORT_WIFI)
            .build()

        val context = SmkexService.getInstance()?.applicationContext ?: return
        val cm = context.getSystemService(CONNECTIVITY_SERVICE) as ConnectivityManager?

        Log.d(TAG, "Init WS connection via WiFi for $clientId")
        cm!!.requestNetwork(wifiNetworkRequest, object : NetworkCallback() {
            override fun onAvailable(network: Network) {
                Log.d(TAG, "Network is available for WiFi")

                val webSocketListener = object : WebSocketListener() {

                    override fun onOpen(webSocket: WebSocket, response: Response) {
                        Log.d(TAG, "Opened WS connection on WIFI: $response")
                    }

                    override fun onMessage(webSocket: WebSocket, text: String) {
                        Log.d(TAG, "Received new string message on WIFI!")
                        SmkexService.onMsgReceived(
                            text.toByteArray(),
                            text.toByteArray().size,
                            channel
                        )
                    }

                    override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
                        Log.d(TAG, "Closing WS connection on WIFI with code $code")
                    }

                    override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                        Log.d(TAG, "Connection on WIFI closed with code $code")
                    }

                    override fun onFailure(
                        webSocket: WebSocket,
                        t: Throwable,
                        response: Response?
                    ) {
                        Log.e(TAG, "Failed to initiate connection on WIFI : $response", t)
                    }
                }

                Log.d("Builder", "About to create new builder")

                webSocketWifiClient =
                    getUnsafeOkHttpClient().socketFactory(network.socketFactory).build()
                webSocketWifiClient?.newWebSocket(request, webSocketListener)
            }

            override fun onUnavailable() {
                super.onUnavailable()
                Log.d(TAG, "Network is NOT available for WiFi")
            }
        })
    }

    private fun initWebSocketConnectionOnCellular(
        clientId: String,
        channel: Int,
        wsServerEndpoint: String?
    ) {
        clientID = clientId

        // Config WS connection for Cellular Mobile Data
        val wsConnectionUrl = "$wsServerEndpoint/$clientId"
        val request = Request.Builder().url(wsConnectionUrl).build()
        val cellularNetworkRequest = NetworkRequest.Builder()
            .addTransportType(NetworkCapabilities.TRANSPORT_CELLULAR)
            .build()

        val context = SmkexService.getInstance()?.applicationContext ?: return
        val cm = context.getSystemService(CONNECTIVITY_SERVICE) as ConnectivityManager?

        Log.d(TAG, "Init WS connection via cellular for $clientId")
        cm!!.requestNetwork(cellularNetworkRequest, object : NetworkCallback() {
            override fun onAvailable(network: Network) {
                Log.d(TAG, "Network is available for Cellular")

                val webSocketListener: WebSocketListener = object : WebSocketListener() {
                    override fun onOpen(webSocket: WebSocket, response: Response) {
                        Log.e(TAG, "Opened WS connection on Cellular: $response")
                    }

                    override fun onMessage(webSocket: WebSocket, text: String) {
                        Log.d(TAG, "Received new string message on Cellular!")
                        SmkexService.onMsgReceived(
                            text.toByteArray(),
                            text.toByteArray().size,
                            channel
                        )
                    }

                    override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
                        Log.d(TAG, "Closing WS connection on cellular with code $code")
                    }

                    override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                        Log.d(TAG, "Connection on cellular closed with code $code")
                    }

                    override fun onFailure(
                        webSocket: WebSocket,
                        t: Throwable,
                        response: Response?
                    ) {
                        Log.e(TAG, "Failed to initiate connection on cellular : $response", t)
                    }
                }

                webSocketCellularClient =
                    getUnsafeOkHttpClient().socketFactory(network.socketFactory).build()
                webSocketCellularClient?.newWebSocket(request, webSocketListener)
            }

            override fun onUnavailable() {
                super.onUnavailable()
                Log.d(TAG, "Network is NOT available for Cellular")
            }
        })
    }

    // Called from JNI
    fun sendOverChannel(buddyId: String, buffer: ByteArray, bufferLen: Int, channel: Int) {
        val transportChannel = channelMapping[channel]
        val httpServerEndpoint = httpServersMappingPerChannel[channel]

        Log.d(
            TAG,
            "Send data for buddy $buddyId via smkex-channel $channel and network-channel $transportChannel"
        )

        when (transportChannel) {
            WIFI_CHANNEL -> sendDataViaWiFi(buddyId, buffer, bufferLen, httpServerEndpoint)
            CELLULAR_CHANNEL -> sendDataViaCellular(buddyId, buffer, bufferLen, httpServerEndpoint)
            else -> Log.e(TAG, "Invalid channel id $channel")
        }
    }

    private fun sendDataViaWiFi(
        buddyId: String,
        buffer: ByteArray,
        bufferLen: Int,
        httpServerEndpoint: String?
    ) {

        // Config HTTP connection for WiFi
        val wifiNetworkRequest = NetworkRequest.Builder()
            .addTransportType(NetworkCapabilities.TRANSPORT_WIFI)
            .build()

        val context = SmkexService.getInstance()?.applicationContext ?: return
        val cm = context.getSystemService(CONNECTIVITY_SERVICE) as ConnectivityManager?

        cm!!.requestNetwork(wifiNetworkRequest, object : NetworkCallback() {
            override fun onAvailable(network: Network) {
                Log.d(TAG, "Successfully created WIFI networking conn.")

                // Customize http client using the created socket factory
                val httpClient = Retrofit.Builder()
                    .baseUrl(httpServerEndpoint)
                    .addConverterFactory(GsonConverterFactory.create())
                    .client(getUnsafeOkHttpClient().apply {
                        socketFactory(network.socketFactory)
                    }.build())
                    .build()
                    .create(ISmkexClientAPI::class.java)

                // Create network request
                val requestBody = buffer.toRequestBody(
                    "application/octet-stream".toMediaTypeOrNull(),
                    0,
                    bufferLen
                )

                val call = httpClient.sendMessage(clientID, buddyId, requestBody)

                // Execute http call async
                call.enqueue(object : Callback<Void> {
                    override fun onResponse(call: Call<Void>, response: retrofit2.Response<Void>) {
                        Log.d(TAG, "HTTP request WIFI succeeded: $response")
                    }

                    override fun onFailure(call: Call<Void>, t: Throwable?) {
                        Log.e(TAG, "HTTP request WIFI failed", t)
                    }
                })
            }
        })
    }

    private fun sendDataViaCellular(
        buddyId: String,
        buffer: ByteArray,
        bufferLen: Int,
        httpServerEndpoint: String?
    ) {

        // Config HTTP connection for Cellular Mobile Data
        val cellularNetworkRequest = NetworkRequest.Builder()
            .addTransportType(NetworkCapabilities.TRANSPORT_CELLULAR)
            .build()

        val context = SmkexService.getInstance()?.applicationContext ?: return
        val cm = context.getSystemService(CONNECTIVITY_SERVICE) as ConnectivityManager?

        cm!!.requestNetwork(cellularNetworkRequest, object : NetworkCallback() {
            override fun onAvailable(network: Network) {
                Log.d(TAG, "Successfully created Cellular networking conn.")

                // Customize http client using the created socket factory
                val httpClient = Retrofit.Builder()
                    .baseUrl(httpServerEndpoint)
                    .addConverterFactory(GsonConverterFactory.create())
                    .client(getUnsafeOkHttpClient().apply {
                        socketFactory(network.socketFactory)
                    }.build())
                    .build()
                    .create(ISmkexClientAPI::class.java)

                // Create network request
                val requestBody = buffer.toRequestBody(
                    "application/octet-stream".toMediaTypeOrNull(),
                    0,
                    bufferLen
                )

                val call = httpClient.sendMessage(clientID, buddyId, requestBody)

                // Execute http call async
                call.enqueue(object : Callback<Void> {
                    override fun onResponse(call: Call<Void>, response: retrofit2.Response<Void>) {
                        Log.d(TAG, "HTTP request Cellular succeeded: $response")
                    }

                    override fun onFailure(call: Call<Void>, t: Throwable?) {
                        Log.e(TAG, "HTTP request Cellular failed", t)
                    }
                })
            }
        })
    }

}