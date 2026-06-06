package com.certsign.smkex.security;

import java.io.IOException;
import java.io.PrintWriter;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.security.core.AuthenticationException;
import org.springframework.security.web.authentication.www.BasicAuthenticationEntryPoint;
import org.springframework.stereotype.Component;

@Component
public class AuthenticationEntryPoint extends BasicAuthenticationEntryPoint {

	private Logger LOG = LoggerFactory.getLogger(AuthenticationEntryPoint.class);

	@Override
	public void commence(HttpServletRequest request, HttpServletResponse response, AuthenticationException authEx)
			throws IOException {
		try {
			response.addHeader("WWW-Authenticate", "Basic realm=" + getRealmName());
			response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
			PrintWriter writer = response.getWriter();
			writer.println("HTTP Status 401 - " + authEx.getMessage());
		} catch (Exception e) {
			LOG.debug("commence Exception", e);
		}
	}

	@Override
	public void afterPropertiesSet() {
		try {
			setRealmName("Smkex");
			super.afterPropertiesSet();
		} catch (Exception e) {
			LOG.debug("afterPropertiesSet Exception", e);
		}
	}
}