package com.certsign.smkex.utils;

import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;

@ConfigurationProperties(prefix = "smkex")
@Configuration("smkexProperties")
public class SmkexProperties {

	// REST API Config
	private String publicZone;
	private String privateZone;

	// Websocket Config
	private String publicWsZone;
	private String privateWsZone;

	// Auth Config
	private String authUsername;
	private String authPassword;

	// Messaging Service Config
	private Integer messageExpireTimeMs;

	public String getPublicWsZone() {
		return publicWsZone;
	}

	public void setPublicWsZone(String publicWsZone) {
		this.publicWsZone = publicWsZone;
	}

	public String getPrivateWsZone() {
		return privateWsZone;
	}

	public void setPrivateWsZone(String privateWsZone) {
		this.privateWsZone = privateWsZone;
	}

	public Integer getMessageExpireTimeMs() {
		return messageExpireTimeMs;
	}

	public void setMessageExpireTimeMs(Integer messageExpireTimeMs) {
		this.messageExpireTimeMs = messageExpireTimeMs;
	}

	public String getPublicZone() {
		return publicZone;
	}

	public void setPublicZone(String publicZone) {
		this.publicZone = publicZone;
	}

	public String getPrivateZone() {
		return privateZone;
	}

	public void setPrivateZone(String privateZone) {
		this.privateZone = privateZone;
	}

	public String getAuthUsername() {
		return authUsername;
	}

	public void setAuthUsername(String authUsername) {
		this.authUsername = authUsername;
	}

	public String getAuthPassword() {
		return authPassword;
	}

	public void setAuthPassword(String authPassword) {
		this.authPassword = authPassword;
	}

}
