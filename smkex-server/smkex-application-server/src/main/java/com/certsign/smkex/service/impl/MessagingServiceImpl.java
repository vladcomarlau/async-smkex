package com.certsign.smkex.service.impl;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;

import com.certsign.smkex.model.Message;
import com.certsign.smkex.service.IMessagingService;
import com.certsign.smkex.utils.Sanitizer;
import com.certsign.smkex.utils.SmkexProperties;

@Service
public class MessagingServiceImpl implements IMessagingService {

	private Logger LOG = LoggerFactory.getLogger(MessagingServiceImpl.class);

	@Autowired
	SmkexProperties smkexProperties;

	// Active users sessions
	private final Object sessionsLock = new Object();
	private final Map<String, WebSocketSession> sessionsMap = new HashMap<>();

	@Override
	public void saveSession(WebSocketSession session) {
		String serial = Sanitizer.getUserSerialFromURI(session.getUri());
		if (Sanitizer.isStringEmpty(serial))
			return;
		synchronized (sessionsLock) {
			sessionsMap.put(serial, session);
		}
	}

	@Override
	public void removeSession(WebSocketSession session) {
		String serial = Sanitizer.getUserSerialFromURI(session.getUri());
		if (Sanitizer.isStringEmpty(serial))
			return;
		synchronized (sessionsLock) {
			sessionsMap.remove(serial);
		}
	}

	@Override
	public void addMessageToQueue(String recipientSerial, Message message) throws IOException {
		LOG.info("Add new message {}, in queue for user {}", message, recipientSerial);

		if (Sanitizer.isStringEmpty(recipientSerial) || !Sanitizer.isMessageValid(message)) {
			LOG.info("Invalid recipient serial or message format");
			return;
		}

		synchronized (sessionsLock) {
			WebSocketSession session = sessionsMap.get(recipientSerial);

			if (session != null) {
				// If the user is connected, the message is sent now
				LOG.info("User {} is online. Send message now.", recipientSerial);
				sendMessage(recipientSerial, message);
			} else {
				// If the user is not connected, the message is added to the queue
				LOG.info("User {} is not connected. Send message when he becomes online.", recipientSerial);
				MessagingQueue.getInstance().addMessageInQueue(recipientSerial, message);
			}
		}
	}

	@Override
	public void removeMessageFromQueue(String recipientSerial, Message message) {
		LOG.info("Remove message {} for user {} from queue.", message, recipientSerial);

		if (Sanitizer.isStringEmpty(recipientSerial))
			return;

		synchronized (sessionsLock) {
			MessagingQueue.getInstance().removeMessageFromQueue(recipientSerial, message);
		}
	}

	@Override
	public void sendMessage(String recipientSerial, Message message) throws IOException {
		LOG.info("Send message {} for user {}", message, recipientSerial);

		if (Sanitizer.isStringEmpty(recipientSerial))
			return;

		synchronized (sessionsLock) {
			WebSocketSession session = sessionsMap.get(recipientSerial);
			if (session != null) {
				session.sendMessage(new TextMessage(message.getContent()));
			}
		}
	}

	@Override
	public void sendMessagesFromQueueForUser(String serial) throws IOException {
		LOG.info("Send all available messages for user {}", serial);

		if (Sanitizer.isStringEmpty(serial))
			return;

		synchronized (sessionsLock) {
			ArrayList<Message> messages = MessagingQueue.getInstance().getAllMessagesForUser(serial);
			LOG.info("User {} has {} pending messages", serial, (messages != null ? messages.size() : 0));

			if (messages == null)
				return;

			for (Message message : messages) {
				// Ignore expired messages
				if (System.currentTimeMillis() - message.getCreationDate() >= smkexProperties.getMessageExpireTimeMs())
					continue;

				sendMessage(serial, message);
			}

			// Clear user queue
			MessagingQueue.getInstance().removeAllMessagesForUser(serial);
		}
	}

	/**
	 * Drain (return and clear) all queued message contents for a user. Used by the
	 * HTTP fetch endpoint so an offline recipient can pull messages when it reconnects.
	 */
	public List<String> drainMessages(String serial) {
		synchronized (sessionsLock) {
			ArrayList<Message> messages = MessagingQueue.getInstance().getAllMessagesForUser(serial);
			List<String> contents = new ArrayList<>();
			if (messages == null)
				return contents;

			for (Message message : messages)
				contents.add(message.getContent());

			MessagingQueue.getInstance().removeAllMessagesForUser(serial);
			return contents;
		}
	}

}
