# TCP echo service with Deno: About

This is simple asynchronous TCP echo server implemented in Typescript/Deno, to demonstrate how Deno services running in Docker containers can be remote-debugged from host machine.

## How to debug

See [main page](../../README.md) for how to run this project.

![image: docker-compose](../../readme-assets/docker-compose-up-dev.png)

After `deno_service` is started in Docker, you can attach VSCode debugger to the running process.

![image: F5](../../readme-assets/deno_service-f5.png)

After you click "Start debugging" or press F5, the debugger will be attached, and you'll see these buttons:

![image: F5 started](../../readme-assets/f5.png)

Put breakpoint to some line of code that works each time a new connection to the service arrives:

![image: breakpoint](../../readme-assets/deno_service-breakpoint.png)

Refresh the `http://localhost:8888/` page, or execute:

```bash
curl --output - 'http://localhost:8888/'
```

And the execution must stop on the breakpoint.

![image: breakpoint](../../readme-assets/deno_service-breakpoint-hit.png)

## How does it work

[Dockerfile](../../infra/java_service/Dockerfile) for this service looks like this:

```dockerfile
FROM denoland/deno:debian-1.16.2 as debug

# 1. To improve build time, copy deps.ts, and cache the app dependencies.
USER deno
COPY ./src/deno_service/private/deps.ts /tmp/deps.ts
RUN deno cache --unstable /tmp/deps.ts
USER root
RUN rm /tmp/deps.ts

# 2. Copy the app
COPY ./src/deno_service /usr/src/deno_service
RUN chown -R root:deno /usr/src/deno_service && \
	chmod -R 750 /usr/src/deno_service

USER deno
CMD ["run", "--unstable", "--allow-net", "--inspect=0.0.0.0:60220", "/usr/src/deno_service/main.ts"]

# app service port
EXPOSE 64840
# debugger port
EXPOSE 60220
```

To start out service, we run `deno` command with debugger parameters. The debugger server (Chrome Debugging Protocol) will be listening on `0.0.0.0:60220` (default route, port 60220).
We expose the debugger port to the host machine together with the app service port.

In [launch.json](../../.vscode/launch.json) we have these settings for the VSCode debugger:

```json
{	"name": "deno_service: Attach to Docker",
	"type": "pwa-node",
	"request": "attach",
	"port": 60220,
	"localRoot": "${workspaceFolder}/src/deno_service",
	"remoteRoot": "/usr/src/deno_service"
}
```

So the debugger client will connect to `localhost:60220`, that is mapped to our service port inside Docker.
