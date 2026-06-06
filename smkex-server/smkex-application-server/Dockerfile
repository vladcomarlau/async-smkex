# Build + run the SMKEX application server as a standalone (embedded Tomcat) jar.
# One image is used for both roles: run one container as the PreKey server
# (path alpha) and another as the Notary server (path beta), on two host ports.

# ---- build stage ----
# JDK 11: the existing controllers use InputStream.readAllBytes() (Java 9+),
# even though the project targets language level 1.8.
FROM maven:3.9-eclipse-temurin-11 AS build
WORKDIR /app
COPY pom.xml .
RUN mvn -q -DskipTests dependency:go-offline || true
COPY src ./src
RUN mvn -q -DskipTests clean package

# ---- run stage ----
FROM eclipse-temurin:11-jre
WORKDIR /app
COPY --from=build /app/target/smkex-application-server.war app.war
EXPOSE 8081
ENTRYPOINT ["java", "-jar", "app.war"]
