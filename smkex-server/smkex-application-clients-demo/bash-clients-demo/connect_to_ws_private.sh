#!/bin/bash

## Check input arguments 
if [ "$#" -ne 3 ]; then
    echo -e "Usage:\t$0 {SERVER_IP} {PORT} {SERIAL}"
    exit 1;
fi

SERVER_IP="$1"
PORT="$2"
SERIAL="$3"

## Connect to the web socket using a default key
## More details here:
## https://en.wikipedia.org/wiki/WebSocket#Protocol_handshake ##

echo "Open WS via PRIVATE path"
curl --include \
     --no-buffer \
     --header "Connection: Upgrade" \
     --header "Upgrade: websocket" \
     --header "Host: localhost:$PORT" \
     --header "Origin: http://localhost:$PORT" \
     --header "Sec-WebSocket-Key: SGVsbG8sIHdvcmxkIQ==" \
     --header "Sec-WebSocket-Version: 13" \
     --header "Authorization: Basic cm9vdDpzbWtleA==" \
     $SERVER_IP:$PORT/private/smkex-websocket/$SERIAL


exit 0;
