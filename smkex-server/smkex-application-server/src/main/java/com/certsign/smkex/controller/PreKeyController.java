package com.certsign.smkex.controller;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import com.certsign.smkex.model.PreKeyBundle;
import com.certsign.smkex.service.impl.PreKeyStore;

/**
 * PreKey server REST endpoints (path alpha).
 *
 * Kept in the /public zone so it can be exercised without authentication during
 * the demo. Restricting uploads/fetches to registered users (e.g. by moving
 * this to the /private zone) is left as future work, see the threat model.
 */
@RestController
@RequestMapping("/public/prekey")
public class PreKeyController {

	private Logger LOG = LoggerFactory.getLogger(PreKeyController.class);

	/** Registration: the recipient uploads his PreKey bundle B. */
	@PostMapping("/{userId}")
	public void uploadBundle(@PathVariable("userId") String userId, @RequestBody PreKeyBundle bundle) {
		int opks = bundle.getOneTimePreKeys() == null ? 0 : bundle.getOneTimePreKeys().size();
		LOG.info("Upload PreKey bundle for {} ({} one-time prekeys)", userId, opks);
		PreKeyStore.getInstance().putBundle(userId, bundle);
	}

	/** Retrieval: the initiator fetches the bundle (one one-time PreKey is consumed). */
	@GetMapping("/{userId}")
	public ResponseEntity<PreKeyBundle> fetchBundle(@PathVariable("userId") String userId) {
		PreKeyBundle bundle = PreKeyStore.getInstance().getBundle(userId);
		if (bundle == null) {
			LOG.info("No PreKey bundle for {}", userId);
			return ResponseEntity.status(HttpStatus.NOT_FOUND).build();
		}

		LOG.info("Serve PreKey bundle for {} (one-time prekey {})", userId,
				bundle.getOneTimePreKey() == null ? "depleted" : "consumed");
		return ResponseEntity.ok(bundle);
	}
}
