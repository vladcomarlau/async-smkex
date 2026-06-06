# SMKEX-Server Tutorial


## Deploy SMKEX server steps:

```sh
# Compile the source code and obtain the JAR file
mvn clean package

# Deploy smkex-server 
java -jar target/smkex-application-server-0.0.1-SNAPSHOT.jar

# Open another terminal and test server connection
wget -qO- http://localhost:8080/probe
```



## Update PORT in configuration file

Change server.port parameter in: 
/src/main/resources/application.properties


