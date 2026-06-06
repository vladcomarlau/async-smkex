package com.certsign.smkex.websocket;

import java.io.IOException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import com.certsign.smkex.service.impl.MessagingServiceImpl;
import com.certsign.smkex.utils.Sanitizer;

@Component
public class SmkexWebsocketHandler extends TextWebSocketHandler {

	@Autowired
	private MessagingServiceImpl messagingService;

	private Logger LOG = LoggerFactory.getLogger(SmkexWebsocketHandler.class);

	@Override
	public void handleTextMessage(WebSocketSession session, TextMessage message)
			throws InterruptedException, IOException {
		LOG.info("New text message from session {} and remote address {}", session.getId(), session.getRemoteAddress());
	}

	@Override
	public void afterConnectionClosed(WebSocketSession session, CloseStatus status) throws Exception {
		LOG.info("Close connection with session {} and remote address {}",
				session.getId() + session.getRemoteAddress());
		messagingService.removeSession(session);
		super.afterConnectionClosed(session, status);
	}

	@Override
	public void afterConnectionEstablished(WebSocketSession session) throws Exception {
		LOG.info("New connection with session {} and remote address {} connected on {}", session.getId(),
				session.getRemoteAddress(), session.getUri());

		super.afterConnectionEstablished(session);

		// Save connection info
		messagingService.saveSession(session);

		// Send messages from queue if exists
		String userSerial = Sanitizer.getUserSerialFromURI(session.getUri());
		messagingService.sendMessagesFromQueueForUser(userSerial);
	}
}