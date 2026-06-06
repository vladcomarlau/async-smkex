package com.certsign.smkex.service.impl;

import java.util.AbstractMap;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.HashMap;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.certsign.smkex.model.PreKeyBundle;

/**
 * In-memory store for PreKey bundles (the PreKey server, path alpha).
 *
 * Follows the same singleton pattern as {@link MessagingQueue}. The long-term
 * part of a bundle is stored as-is; the one-time PreKeys are kept in a separate
 * per-user queue of (id, value) pairs so that each fetch consumes (pops) one of
 * them. The id is just the upload position (0, 1, 2, ...) and tells the initiator
 * which attestation to request from the Notary server.
 *
 * If the pool is empty the bundle is withheld (the fetch returns nothing), so a
 * served bundle always carries exactly one one-time PreKey with a matching
 * attestation. Graceful no-OPK fallback is left as future work.
 */
public class PreKeyStore {

	private Logger LOG = LoggerFactory.getLogger(PreKeyStore.class);

	private final Object lock = new Object();
	private final Map<String, PreKeyBundle> bundles = new HashMap<>();
	private final Map<String, Deque<Map.Entry<String, String>>> oneTimeKeys = new HashMap<>();

	// the singleton dp
	private static PreKeyStore instance;

	private PreKeyStore() {
	}

	public static synchronized PreKeyStore getInstance() {
		if (instance == null)
			instance = new PreKeyStore();

		return instance;
	}

	/** Store (or replace) a user's bundle and seed its one-time PreKey pool. */
	public void putBundle(String userId, PreKeyBundle bundle) {
		synchronized (lock) {
			bundles.put(userId, bundle);

			Deque<Map.Entry<String, String>> pool = new ArrayDeque<>();
			if (bundle.getOneTimePreKeys() != null) {
				int id = 0;
				for (String opk : bundle.getOneTimePreKeys()) {
					pool.add(new AbstractMap.SimpleEntry<>(String.valueOf(id), opk));
					id++;
				}
			}
			oneTimeKeys.put(userId, pool);

			LOG.info("Stored bundle for {} with {} one-time prekeys (ids 0..{})", userId, pool.size(),
					pool.size() - 1);
		}
	}

	/**
	 * Return a user's bundle with one consumed one-time PreKey (and its id).
	 * Returns {@code null} if the user has no bundle, or if the one-time PreKey
	 * pool is depleted (the bundle is then withheld until the user replenishes).
	 */
	public PreKeyBundle getBundle(String userId) {
		synchronized (lock) {
			PreKeyBundle stored = bundles.get(userId);
			if (stored == null) {
				LOG.info("No bundle for {}", userId);
				return null;
			}

			Deque<Map.Entry<String, String>> pool = oneTimeKeys.get(userId);
			if (pool == null || pool.isEmpty()) {
				LOG.info("One-time prekey pool depleted for {} - bundle withheld", userId);
				return null;
			}

			Map.Entry<String, String> opk = pool.poll();

			PreKeyBundle response = new PreKeyBundle();
			response.setIdentityKeyEd25519(stored.getIdentityKeyEd25519());
			response.setIdentityKeyX25519(stored.getIdentityKeyX25519());
			response.setSignedPreKey(stored.getSignedPreKey());
			response.setSignedPreKeySignature(stored.getSignedPreKeySignature());
			response.setTimestamp(stored.getTimestamp());
			response.setOneTimePreKeyId(opk.getKey());
			response.setOneTimePreKey(opk.getValue());

			LOG.info("Serve bundle for {} with one-time prekey id {}", userId, opk.getKey());
			return response;
		}
	}
}
