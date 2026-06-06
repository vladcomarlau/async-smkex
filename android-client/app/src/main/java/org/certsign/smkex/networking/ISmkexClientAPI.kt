package org.certsign.smkex.networking

import okhttp3.RequestBody
import retrofit2.Call
import retrofit2.http.Body
import retrofit2.http.POST
import retrofit2.http.Path


interface ISmkexClientAPI {
    
    @POST("/public/send-message/{clientID}/{destinationID}")
    fun sendMessage(
        @Path("clientID") clientID: String?,
        @Path("destinationID") destinationID: String?,
        @Body message: RequestBody
    ): Call<Void>

}