#!/bin/bash

# Smoke test for the Async-SMKEX PreKey + Notary endpoints (Milestone 1, option b).
#
# Registers a recipient ("bob"):
#   - the bundle (core + a list of one-time PreKeys) on the PreKey server (path alpha)
#   - one attestation PER one-time PreKey, keyed by id, on the Notary server (path beta)
# Then fetches as an initiator would: a bundle fetch returns one one-time PreKey
# AND its id; the matching attestation is then fetched from the notary by that id.
#
# The key/attestation values are dummy Base64 placeholders - the servers do not
# validate cryptographic material, they only store and serve opaque strings.
#
# Usage:  ./async_smoke_test.sh {PREKEY_HOST} {PREKEY_PORT} {NOTARY_HOST} {NOTARY_PORT}
# Example: ./async_smoke_test.sh localhost 8081 localhost 8082

if [ "$#" -ne 4 ]; then
    echo -e "Usage:\t$0 {PREKEY_HOST} {PREKEY_PORT} {NOTARY_HOST} {NOTARY_PORT}"
    exit 1;
fi

PREKEY="http://$1:$2"
NOTARY="http://$3:$4"
USER="bob"

echo -e "\n++++ 1. Registration: upload bundle (2 one-time prekeys) to PreKey server (alpha) ++++"
curl -s -X POST "$PREKEY/public/prekey/$USER" \
    -H "Content-Type: application/json" \
    -d '{
        "identityKeyEd25519":"SUtfRWQ=",
        "identityKeyX25519":"SUtfWA==",
        "signedPreKey":"U1BL",
        "signedPreKeySignature":"c2ln",
        "timestamp":1730000000,
        "oneTimePreKeys":["T1BLMQ==","T1BLMg=="]
    }'

echo -e "\n++++ 2. Registration: upload one attestation per one-time prekey id to Notary (beta) ++++"
curl -s -X POST "$NOTARY/public/notary/$USER" \
    -H "Content-Type: application/json" \
    -d '{"0":"QXR0ZXN0YXRpb25fMA==","1":"QXR0ZXN0YXRpb25fMQ=="}'

echo -e "\n\n++++ 3. Retrieval: fetch bundle (consumes one-time prekey id 0) ++++"
curl -s "$PREKEY/public/prekey/$USER"; echo
echo -n "    matching attestation (notary id 0): "; curl -s "$NOTARY/public/notary/$USER/0"; echo

echo -e "\n++++ 4. Retrieval again: fetch bundle (consumes one-time prekey id 1) ++++"
curl -s "$PREKEY/public/prekey/$USER"; echo
echo -n "    matching attestation (notary id 1): "; curl -s "$NOTARY/public/notary/$USER/1"; echo

echo -e "\n++++ 5. Pool depleted: third fetch is withheld (HTTP 404) ++++"
echo -n "    bundle fetch #3 status: "; curl -s -o /dev/null -w "%{http_code}\n" "$PREKEY/public/prekey/$USER"

echo -e "\nDone."
exit 0;
