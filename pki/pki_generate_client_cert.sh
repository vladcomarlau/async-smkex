#!/bin/bash

echo "******** Create client certificate ********"

echo "Please enter the client full name (with no spaces):"
 
read fullname

FULLNAME=$fullname openssl req -out ca/intermediate-server/csr/$fullname.csr.pem -newkey rsa:2048 -nodes -keyout ca/intermediate-server/private/$fullname.key.pem -config openssl_client.cnf
openssl ca -config openssl_intermediate_server_subca.cnf -days 1800 -notext -md sha512 -in ca/intermediate-server/csr/$fullname.csr.pem -out ca/intermediate-server/certs/$fullname.crt.pem

mkdir -p artifacts/clients/$fullname
cp ca/intermediate-server/private/$fullname.key.pem artifacts/clients/$fullname
cp ca/intermediate-server/certs/$fullname.crt.pem artifacts/clients/$fullname
cat ca/intermediate-server/certs/int.smkex-server-subca.crt.pem ca/certs/ca.smkex-root.crt.pem > artifacts/clients/$fullname/$fullname.chain.pem

# Generate PKCS12
openssl pkcs12 -export -out artifacts/clients/$fullname/$fullname.p12 -inkey artifacts/clients/$fullname/$fullname.key.pem -in artifacts/clients/$fullname/$fullname.crt.pem -certfile artifacts/clients/$fullname/$fullname.chain.pem

