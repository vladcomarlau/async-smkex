#!/bin/bash
#
# Build the SMKEX application-server image and start the two Async-SMKEX servers:
#   - PreKey server (path alpha)
#   - Notary server (path beta)
# each as its own container on its own host port.
#
# Uses the self-contained Dockerfile in smkex-application-server/ (builds the WAR
# from source, runs on embedded Tomcat). This is NOT the production build.sh flow.
#
# Usage:  ./run_async_servers.sh [PREKEY_PORT] [NOTARY_PORT]
# Default ports: 8081 (PreKey), 8082 (Notary)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$SCRIPT_DIR/smkex-application-server"

PREKEY_PORT="${1:-8081}"
NOTARY_PORT="${2:-8082}"
IMAGE="smkex-async"

echo "******* Build image ($IMAGE) *******"
# cd into the build context and use "." so it works from both WSL and Git-Bash
( cd "$APP_DIR" && docker build -t "$IMAGE" . )

echo ""
echo "******* Remove previous containers (if any) *******"
docker rm -f prekey notary 2>/dev/null || true

echo ""
echo "******* Start servers *******"
docker run -d -p "$PREKEY_PORT":8081 --name prekey "$IMAGE" >/dev/null
docker run -d -p "$NOTARY_PORT":8081 --name notary "$IMAGE" >/dev/null
echo "PreKey (path alpha) -> http://localhost:$PREKEY_PORT"
echo "Notary (path beta)  -> http://localhost:$NOTARY_PORT"

# Wait until a server answers /public/probe (startup is ~5s)
wait_up() {
    local label="$1" port="$2" container="$3"
    if curl -s --retry 90 --retry-delay 1 --retry-all-errors "http://localhost:$port/public/probe" >/dev/null; then
        echo "  $label up on $port: $(curl -s http://localhost:$port/public/probe)"
    else
        echo "  WARNING: $label did not respond on $port. Recent logs:"
        docker logs --tail 15 "$container"
    fi
}

echo ""
echo "******* Wait for servers to come up *******"
wait_up "PreKey" "$PREKEY_PORT" prekey
wait_up "Notary" "$NOTARY_PORT" notary

echo ""
echo "******* Running *******"
docker ps --filter name=prekey --filter name=notary --format "  {{.Names}}  {{.Status}}  {{.Ports}}"
echo ""
echo "Smoke test : bash \"$SCRIPT_DIR/smkex-application-clients-demo/bash-clients-demo/async_smoke_test.sh\" localhost $PREKEY_PORT localhost $NOTARY_PORT"
echo "Stop both  : docker rm -f prekey notary"

exit 0
