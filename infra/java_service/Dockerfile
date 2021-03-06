# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM maven:3.8.3-jdk-11 as debug

# 1. Create user and group for the service.
#	I use 1205 as user and group id.
#	I think giving different user ids to different services improves isolation.
RUN addgroup --gid=1205 java_service_user && \
	adduser --uid=1205 --gid=1205 --shell /bin/false --system java_service_user

# 2. Source code will be copied and compiled in this directory.
#	Later i'll delete it.
WORKDIR /usr/src/java_service

# 3. Cache dependencies
COPY ./src/java_service/pom.xml .
RUN mvn verify clean --fail-never

# 4. Build
COPY ./src/java_service .
RUN mvn -B package && \
	mkdir /usr/lib/java_service && \
	mv target/java_service-1.0-SNAPSHOT.jar /usr/lib/java_service/java_service.jar && \
	chown -R root:java_service_user /usr/lib/java_service && \
	chmod -R 750 /usr/lib/java_service && \
	rm -rf /usr/src/java_service

# 5. How to run the app.
WORKDIR /home/java_service_user
USER java_service_user
ENV MAVEN_CONFIG=/home/java_service_user
CMD ["java", "-agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=*:9455", "-cp", "/usr/lib/java_service/java_service.jar", "com.none.java_service.App"]

# app service port = 27712
# debugger port = 9455
EXPOSE 27712 9455


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
COPY --from=debug /usr/lib/java_service /usr/lib/java_service

# 3. How to run the app.
WORKDIR /home/java_service_user
USER java_service_user
CMD ["java", "-cp", "/usr/lib/java_service/java_service.jar", "com.none.java_service.App"]

# app service port = 27712
EXPOSE 27712
