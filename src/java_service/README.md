## About this service

This is simple asynchronous TCP echo server implemented in Java, to demonstrate how Java services running in Docker containers can be debugged from host machine.

Files here were generated like this:

```bash
mvn archetype:generate -DgroupId=com.none.java_service -DartifactId=java_service -DarchetypeArtifactId=maven-archetype-quickstart -DarchetypeVersion=1.4 -DinteractiveMode=false
```

Then the service implementation was written in [src/main/java/com/none/java_service/App.java](./src/main/java/com/none/java_service/App.java).

## How to debug

See [main page](../../README.md) for how to run this project.

![image: docker-compose](../../readme-assets/docker-compose-up-dev.png)

After `java_service` is started in Docker, you can attach VSCode debugger to the running process.

![image: F5](../../readme-assets/java_service-f5.png)

After you click "Start debugging" or press F5, the debugger will be attached, and you'll see these buttons:

![image: F5 started](../../readme-assets/f5-with-patch.png)

Put breakpoint to some line of code that works each time a new connection to the service arrives:

```java
System.out.println("Conn");
```

![image: breakpoint](../../readme-assets/java_service-breakpoint.png)

Refresh the `http://localhost:8888/` page, or execute:

```bash
curl --output - 'http://localhost:8888/'
```

And the execution must stop on the breakpoint.

![image: breakpoint](../../readme-assets/java_service-breakpoint-hit.png)

## How does it work

[Dockerfile](../../infra/java_service/Dockerfile) for this service looks like this:

```dockerfile
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
```

To start out service, we run `java` command with debugger parameters. The Java debugger server will be listening on `*:9455` (default route, port 9455).
We expose the debugger port to the host machine together with the app service port.

In [launch.json](../../.vscode/launch.json) we have these settings for the VSCode debugger:

```json
{	"name": "java_service: Attach to Docker",
	"type": "java",
	"request": "attach",
	"hostName": "localhost",
	"port": 9455,
	"projectName": "java_service",
	"sourcePaths": ["${workspaceFolder}/src/java_service/src/main/java"]
}
```

So the debugger client will connect to `localhost:9455`, that is mapped to our serice port inside Docker.

`"request": "attach"` tells the debugger to attach to already running process, not to launch a new application.

## See also

- [Running and debugging Java - Attach](https://code.visualstudio.com/docs/java/java-debugging#_attach)
- [What are Java command line options to set to allow JVM to be remotely debugged?](https://stackoverflow.com/questions/138511/what-are-java-command-line-options-to-set-to-allow-jvm-to-be-remotely-debugged)