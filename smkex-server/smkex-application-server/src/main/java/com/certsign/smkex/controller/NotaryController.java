package com.certsign.smkex.controller;

import java.util.Map;

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

import com.certsign.smkex.service.impl.NotaryStore;

/**
 * Notary server REST endpoints (path beta).
 *
 * Stores one attestation per one-time PreKey id, each A_i = Sig(IK,
 * SHA-256(core || OPK_i)) as an opaque Base64 string. Kept in the /public zone
 * for the demo; restricting to registered users is left as future work (see the
 * threat model).
 */
@RestController
@RequestMapping("/public/notary")
public class NotaryController {

	private Logger LOG = LoggerFactory.getLogger(NotaryController.class);

	/** Registration: the recipient uploads all attestations, keyed by one-time PreKey id. */
	@PostMapping("/{userId}")
	public void uploadAttestations(@PathVariable("userId") String userId,
			@RequestBody Map<String, String> attestations) {
		LOG.info("Upload {} attestations for {}", attestations.size(), userId);
		NotaryStore.getInstance().putAll(userId, attestations);
	}

	/** Retrieval: the initiator fetches the attestation matching the one-time PreKey id. */
	@GetMapping("/{userId}/{keyId}")
	public ResponseEntity<String> fetchAttestation(@PathVariable("userId") String userId,
			@PathVariable("keyId") String keyId) {
		String attestation = NotaryStore.getInstance().get(userId, keyId);
		if (attestation == null) {
			LOG.info("No attestation for {} id {}", userId, keyId);
			return ResponseEntity.status(HttpStatus.NOT_FOUND).build();
		}

		LOG.info("Serve attestation for {} id {}", userId, keyId);
		return ResponseEntity.ok(attestation);
	}

	/** Retrieval (concurrent path): fetch all of a user's attestations in one request. */
	@GetMapping("/{userId}")
	public ResponseEntity<Map<String, String>> fetchAllAttestations(@PathVariable("userId") String userId) {
		Map<String, String> all = NotaryStore.getInstance().getAll(userId);
		if (all == null || all.isEmpty()) {
			LOG.info("No attestations for {}", userId);
			return ResponseEntity.status(HttpStatus.NOT_FOUND).build();
		}

		LOG.info("Serve {} attestations for {}", all.size(), userId);
		return ResponseEntity.ok(all);
	}
}
