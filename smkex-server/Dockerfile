FROM tomcat:8.5-jdk11-corretto

# Copy WAR file in webapps
COPY ROOT.war /usr/local/tomcat/webapps/ROOT.war
COPY configuration/tomcat/server.xml /usr/local/tomcat/conf/server.xml
COPY smkex-server.crt /usr/local/tomcat/conf
COPY smkex-server.priv /usr/local/tomcat/conf

# Start Tomcat 
RUN /usr/local/tomcat/bin/startup.sh
