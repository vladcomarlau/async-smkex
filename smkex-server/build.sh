#!/bin/bash

# Check input arguments 
if [ "$#" -ne 2 ]; then
    echo -e "Usage:\t$0 {SECURE_PORT UNSECURE_PORT}"
    exit 1;
fi

SECURE_PORT="$1"
UNSECURE_PORT="$2"

# Build WAR file
echo -e "\n\n******* Build WAR File *******"
cd smkex-application-server && \
	mvn clean package -DskipTests=true && \
	mv target/smkex-application-server.war ../ROOT.war


# Stop Previous Docker Container
echo -e "\n\n******* Stop Previous Docker Container If Exists *******"
docker stop smkex-server
docker rm smkex-server

# Delete Previous Docker Image
echo -e "\n\n******* Delete Previous Docker Image *******"
docker image rm smkex-docker

# Create certificate
echo -e "\n\n******* Create server certificate *******"
cd .. && openssl req -x509 -newkey rsa:4096 -keyout ./smkex-server.priv -out ./smkex-server.crt -sha256 -days 1000 -subj '/CN=smkex-server' -nodes

# Create Docker Image
echo -e "\n\n******* Create Docker Image *******"
docker build --rm -t smkex-docker . 

# Run Docker Container
echo -e "\n\n******* Start Docker Container *******"
docker run -d -p $SECURE_PORT:8443 -p $UNSECURE_PORT:8080 --name smkex-server smkex-docker 

# Display status
docker ps -a | grep -i "smkex-server"

# Perform Clean-Up
echo -e "\n\n******* Perform Clean-Up *******"
rm -rf *.war

exit 0;
