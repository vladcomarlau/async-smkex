package com.certsign.smkex.model;

import java.util.Objects;

public class Message {

	// Message Session ID (TODO: This is NOT USED!!)
	private long sessionID;
	
	// The sender of the message
	private String senderSerial;
	
	// Message content 
	private String content;
	
	// Message creation date (used for expiration time checks)
	private long creationDate;

	public Message(long sessionID, String content, String senderSerial) {
		super();
		this.sessionID = sessionID;
		this.senderSerial = senderSerial;
		this.content = content;
		this.creationDate = System.currentTimeMillis();
	}

	public long getSessionID() {
		return sessionID;
	}

	public void setSessionID(long sessionID) {
		this.sessionID = sessionID;
	}

	public String getContent() {
		return content;
	}

	public void setContent(String content) {
		this.content = content;
	}

	public long getCreationDate() {
		return creationDate;
	}

	public void setCreationDate(long creationDate) {
		this.creationDate = creationDate;
	}

	public String getSenderSerial() {
		return senderSerial;
	}

	public void setSenderSerial(String senderSerial) {
		this.senderSerial = senderSerial;
	}

	@Override
	public int hashCode() {
		return Objects.hash(sessionID, senderSerial, content);
	}

	@Override
	public boolean equals(Object obj) {
		if (obj == null)
			return false;
		if (this == obj)
			return true;

		if ((obj instanceof Message) && (((Message) obj).getSessionID() == this.sessionID)
				&& (((Message) obj).getSenderSerial().equals(this.senderSerial))
				&& (((Message) obj).getContent().equals(this.content)))
			return true;
		else
			return false;
	}

	@Override
	public String toString() {
		return "Message{ sessionID: " + sessionID
				+ ", senderSerial: " + senderSerial
				+ ", content: " + content
				+ ", creationDate: " + creationDate + " }";
	}
}
