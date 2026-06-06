package org.certsign.smkex.utils.messages

data class MessageReceivedData(
        val senderUri: String?,
        val messageContent: ByteArray?,
        val messageLen: Int?
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as MessageReceivedData

        if (senderUri != other.senderUri) return false
        if (messageContent != null) {
            if (other.messageContent == null) return false
            if (!messageContent.contentEquals(other.messageContent)) return false
        } else if (other.messageContent != null) return false
        if (messageLen != other.messageLen) return false

        return true
    }

    override fun hashCode(): Int {
        var result = senderUri?.hashCode() ?: 0
        result = 31 * result + (messageContent?.contentHashCode() ?: 0)
        result = 31 * result + (messageLen ?: 0)
        return result
    }
}