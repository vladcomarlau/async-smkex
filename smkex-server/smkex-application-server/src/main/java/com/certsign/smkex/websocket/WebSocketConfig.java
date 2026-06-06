package com.certsign.smkex.websocket;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Configuration;
import org.springframework.web.socket.config.annotation.EnableWebSocket;
import org.springframework.web.socket.config.annotation.WebSocketConfigurer;
import org.springframework.web.socket.config.annotation.WebSocketHandlerRegistry;

import com.certsign.smkex.utils.SmkexProperties;

@Configuration
@EnableWebSocket
public class WebSocketConfig implements WebSocketConfigurer {
	private Logger LOG = LoggerFactory.getLogger(WebSocketConfig.class);

	@Autowired
	private SmkexProperties smkexProperties;
	@Autowired
	private SmkexWebsocketHandler handler;

	@Override
	public void registerWebSocketHandlers(WebSocketHandlerRegistry registry) {
		LOG.info("Register websocket handler for paths: {}, {}", smkexProperties.getPublicWsZone(),
				smkexProperties.getPrivateWsZone());
		registry.addHandler(handler, smkexProperties.getPublicWsZone(), smkexProperties.getPrivateWsZone());
	}

}
