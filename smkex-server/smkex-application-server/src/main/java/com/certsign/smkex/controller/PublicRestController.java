package com.certsign.smkex.controller;

import java.io.IOException;
import java.util.Date;
import java.util.List;

import javax.servlet.http.HttpServletRequest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import com.certsign.smkex.model.Message;
import com.certsign.smkex.service.impl.MessagingServiceImpl;

@RestController
@RequestMapping("/public")
public class PublicRestController {

	private Logger LOG = LoggerFactory.getLogger(PrivateRestController.class);

	@Autowired
	private MessagingServiceImpl messagingService;

	@GetMapping("/probe")
	public String probe() {
		return "Current Date: " + new Date().toString();
	}

	@PostMapping("/send-message/{senderSerial}/{recipientSerial}")
	public void sendMessage(HttpServletRequest request, @PathVariable("senderSerial") String senderSerial,
			@PathVariable("recipientSerial") String recipientSerial) throws IOException {

		LOG.info("Send new message from {} to {}", senderSerial, recipientSerial);

		// Parse data
		byte[] data = request.getInputStream().readAllBytes();

		// Add message to queue
		messagingService.addMessageToQueue(recipientSerial, new Message(0L, new String(data), senderSerial));
	}

	@GetMapping("/messages/{userId}")
	public List<String> fetchMessages(@PathVariable("userId") String userId) {
		LOG.info("Fetch (drain) queued messages for {}", userId);
		return messagingService.drainMessages(userId);
	}
}
