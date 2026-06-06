#!/bin/bash


## Check input arguments 
if [ "$#" -ne 5 ]; then
    echo -e "Usage:\t$0 {SERVER_IP} {PORT} {SOURCE_SERIAL} {DESTINATION_SERIAL} {MSG_CONTENT}"
    exit 1;
fi


## Init parameters 
SESSION_ID=`date +%s`

SERVER_IP="$1"
PORT="$2"
SOURCE_SERIAL="$3"
DESTINATION_SERIAL="$4"
CONTENT="$5"

## Perform HTTP POST request in order to send a message 
echo -e "\n\n++++ Send via PRIVATE path ++++"
curl 	-v -X POST -d "$CONTENT from Private" \
	http://$SERVER_IP:$PORT/private/send-message/$SOURCE_SERIAL/$DESTINATION_SERIAL \
	-u root:smkex

exit 0;

