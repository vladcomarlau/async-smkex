package com.certsign.smkex.service.impl;

import java.util.HashMap;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * In-memory store for bundle attestations (the Notary server, path beta).
 *
 * For each user it holds a small map of (one-time PreKey id -> attestation),
 * where each attestation is A_i = Sig(IK, SHA-256(core || OPK_i)) stored as an
 * opaque Base64 string. The initiator fetches the attestation whose id matches
 * the one-time PreKey it received from the PreKey server. The notary never
 * receives the bundle and never signs anything, so it cannot forge attestations.
 */
public class NotaryStore {

	private Logger LOG = LoggerFactory.getLogger(NotaryStore.class);

	private final Object lock = new Object();
	private final Map<String, Map<String, String>> attestations = new HashMap<>();

	private static NotaryStore instance;

	private NotaryStore() {
	}

	public static synchronized NotaryStore getInstance() {
		if (instance == null)
			instance = new NotaryStore();

		return instance;
	}

	/** Store (or replace) all of a user's attestations, keyed by one-time PreKey id. */
	public void putAll(String userId, Map<String, String> idToAttestation) {
		synchronized (lock) {
			attestations.put(userId, new HashMap<>(idToAttestation));
			LOG.info("Stored {} attestations for {}", idToAttestation.size(), userId);
		}
	}

	/** Return the attestation for a given one-time PreKey id, or {@code null}. */
	public String get(String userId, String keyId) {
		synchronized (lock) {
			Map<String, String> userAttestations = attestations.get(userId);
			return userAttestations == null ? null : userAttestations.get(keyId);
		}
	}

	/** Return all of a user's attestations (id -> attestation), or {@code null}. */
	public Map<String, String> getAll(String userId) {
		synchronized (lock) {
			Map<String, String> userAttestations = attestations.get(userId);
			return userAttestations == null ? null : new HashMap<>(userAttestations);
		}
	}
}
