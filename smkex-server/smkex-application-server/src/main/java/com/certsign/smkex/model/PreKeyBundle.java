package com.certsign.smkex.model;

import java.util.List;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;

/**
 * A PreKey bundle as defined in the Async-SMKEX design (bundle B).
 *
 * The server treats every key as an opaque Base64 string; it never interprets
 * or validates the cryptographic material. The two identity keys are kept
 * separate on purpose: the Ed25519 key signs/verifies the attestation and the
 * signed PreKey, while the X25519 key is used for the Diffie-Hellman steps.
 *
 * On upload the client sends a list of one-time PreKeys ({@code oneTimePreKeys}).
 * On fetch the server returns the long-term fields plus a single consumed
 * one-time PreKey ({@code oneTimePreKey}) together with its id
 * ({@code oneTimePreKeyId}); the id tells the initiator which attestation to
 * request from the Notary server.
 */
@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown = true)
public class PreKeyBundle {

	private String identityKeyEd25519;     // IK (Ed25519) - signs attestation and signed PreKey
	private String identityKeyX25519;      // IK (X25519)  - used in DH1, DH2
	private String signedPreKey;           // SPK (X25519)
	private String signedPreKeySignature;  // Sig(IK_ed, SPK)
	private long timestamp;                 // bundle creation time (freshness)
	private List<String> oneTimePreKeys;   // OPK pool (upload only)
	private String oneTimePreKey;          // single consumed OPK (fetch only)
	private String oneTimePreKeyId;        // id of the consumed OPK (fetch only)

	public String getIdentityKeyEd25519() {
		return identityKeyEd25519;
	}

	public void setIdentityKeyEd25519(String identityKeyEd25519) {
		this.identityKeyEd25519 = identityKeyEd25519;
	}

	public String getIdentityKeyX25519() {
		return identityKeyX25519;
	}

	public void setIdentityKeyX25519(String identityKeyX25519) {
		this.identityKeyX25519 = identityKeyX25519;
	}

	public String getSignedPreKey() {
		return signedPreKey;
	}

	public void setSignedPreKey(String signedPreKey) {
		this.signedPreKey = signedPreKey;
	}

	public String getSignedPreKeySignature() {
		return signedPreKeySignature;
	}

	public void setSignedPreKeySignature(String signedPreKeySignature) {
		this.signedPreKeySignature = signedPreKeySignature;
	}

	public long getTimestamp() {
		return timestamp;
	}

	public void setTimestamp(long timestamp) {
		this.timestamp = timestamp;
	}

	public List<String> getOneTimePreKeys() {
		return oneTimePreKeys;
	}

	public void setOneTimePreKeys(List<String> oneTimePreKeys) {
		this.oneTimePreKeys = oneTimePreKeys;
	}

	public String getOneTimePreKey() {
		return oneTimePreKey;
	}

	public void setOneTimePreKey(String oneTimePreKey) {
		this.oneTimePreKey = oneTimePreKey;
	}

	public String getOneTimePreKeyId() {
		return oneTimePreKeyId;
	}

	public void setOneTimePreKeyId(String oneTimePreKeyId) {
		this.oneTimePreKeyId = oneTimePreKeyId;
	}
}
