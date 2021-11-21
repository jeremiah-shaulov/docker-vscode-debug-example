# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM maven:3.8.3-jdk-11 as debug

WORKDIR /usr/src/java_service

# Cache dependencies
COPY ./src/java_service/pom.xml .
RUN mvn -B dependency:go-offline --fail-never

# Build
COPY ./src/java_service .
RUN mvn -B package

CMD ["java", "-agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=*:9455", "-cp", "target/java_service-1.0-SNAPSHOT.jar", "com.none.java_service.App"]

# app service port
EXPOSE 27712
# debugger port
EXPOSE 9455


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM openjdk:11.0.13-jre-slim-bullseye

# 1. Create user and group for the service.
#	I use 1205 as user and group id.
#	I think giving different user ids to different services improves isolation.
RUN addgroup --gid=1205 java_service_user && \
	adduser --uid=1205 --gid=1205 --shell /bin/false --system java_service_user

# 2. Copy the binary that compiled on the previous stage to this new Debian image.
COPY --from=debug /usr/src/java_service/target/java_service-1.0-SNAPSHOT.jar /usr/lib/java_service/java_service.jar

# 3. How to run the app.
USER java_service_user
CMD ["java", "-cp", "/usr/lib/java_service/java_service.jar", "com.none.java_service.App"]

# app service port
EXPOSE 27712
