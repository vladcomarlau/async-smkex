package com.certsign.smkex.service.impl;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.certsign.smkex.model.Message;
import com.certsign.smkex.utils.Sanitizer;

public class MessagingQueue {

	private Logger LOG = LoggerFactory.getLogger(MessagingQueue.class);

	// Lock used for multithreading access in queue map
	private final Object queueLock;
	private Map<String, ArrayList<Message>> queueMap;

	// Singleton Pattern
	private static MessagingQueue instance;

	private MessagingQueue() {
		queueLock = new Object();
		queueMap = new HashMap<>();
	}

	public static synchronized MessagingQueue getInstance() {
		if (instance == null)
			instance = new MessagingQueue();

		return instance;
	}

	public boolean addMessageInQueue(String recipientSerial, Message message) {
		if (Sanitizer.isStringEmpty(recipientSerial) || !Sanitizer.isMessageValid(message))
			return false;

		synchronized (queueLock) {
			// Remove messages with an older session
			removeMessagesFromOlderSessions(message.getSenderSerial(), recipientSerial, message.getSessionID());

			// Add message in user queue
			ArrayList<Message> messages = queueMap.get(recipientSerial);
			if (messages == null) {
				messages = new ArrayList<>();
				queueMap.put(recipientSerial, messages);
			}
			
			messages.add(message);

			return true;
		}
	}

	public void removeMessagesFromOlderSessions(String senderSerial, String recipientSerial, long newSessionID) {
		synchronized (queueLock) {
			ArrayList<Message> messages = queueMap.get(recipientSerial);
			if (messages == null)
				return;

			ArrayList<Message> cMessages = new ArrayList<>(messages);

			for (Message message : cMessages) {
				if (message.getSenderSerial().equals(senderSerial) && message.getSessionID() < newSessionID) {
					LOG.info("Remove message from " + senderSerial + " sent to " + recipientSerial
							+ " with an older session ID: " + message.getSessionID());
					removeMessageFromQueue(recipientSerial, message);
				}
			}
		}
	}

	public boolean removeMessageFromQueue(String recipientSerial, Message message) {
		if (Sanitizer.isStringEmpty(recipientSerial) || !Sanitizer.isMessageValid(message))
			return false;

		synchronized (message) {
			ArrayList<Message> messages = queueMap.get(recipientSerial);

			if (messages == null || !messages.contains(message))
				return false;

			// Remove message from user queue
			messages.remove(message);

			return true;
		}
	}

	public ArrayList<Message> getAllMessagesForUser(String serial) {
		if (Sanitizer.isStringEmpty(serial))
			return null;

		synchronized (queueLock) {
			return queueMap.get(serial);
		}
	}

	public boolean removeAllMessagesForUser(String serial) {
		if (Sanitizer.isStringEmpty(serial))
			return false;

		ArrayList<Message> allMessages = getAllMessagesForUser(serial);
		if (allMessages == null)
			return true;

		ArrayList<Message> cAllMessages = new ArrayList<>(allMessages);

		for (Message message : cAllMessages) {
			removeMessageFromQueue(serial, message);
		}

		return true;

	}
}
