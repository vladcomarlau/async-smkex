package com.certsign.smkex.service;

import java.io.IOException;

import org.springframework.web.socket.WebSocketSession;

import com.certsign.smkex.model.Message;

public interface IMessagingService {

	
	/**
	 * Save user session after the ws connection is established
	 * @param serial
	 * @param session
	 */
	void saveSession(WebSocketSession session);
	
	/**
	 * Remove user session after the ws connection is closed
	 * @param serial
	 */
	void removeSession(WebSocketSession session);
	
	/**
	 * Add message in user queue
	 * 
	 * @param destinationSerial
	 * @param message
	 * @throws IOException 
	 */
	void addMessageToQueue(String destinationSerial, Message message) throws IOException;

	/**
	 * Remove message from user queue
	 * 
	 * @param destinationSerial
	 * @param message
	 */
	void removeMessageFromQueue(String destinationSerial, Message message);

	/**
	 * Get all messages from queue for the current serial
	 * 
	 * @param session
	 * @param serial
	 * @throws IOException 
	 */
	void sendMessagesFromQueueForUser(String serial) throws IOException;

	/**
	 * Send message to user when he becomes available
	 * 
	 * @param session
	 * @param destinationSerial
	 * @param message
	 * @throws IOException 
	 */
	void sendMessage(String destinationSerial, Message message) throws IOException;

}
