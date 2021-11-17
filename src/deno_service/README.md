# TCP echo service with Deno

This little Docker service demonstrates the ability to remote-debug application running in a Docker container from host machine.

See main [page](../../) on how to run this project. Here i'll explain what the debugging process looks like.

When `dev` version is started, the command that runs inside the container looks like this: (find it in [docker-compose.dev.yaml](../docker-compose.dev.yaml))

```dockerfile
  deno_service:
    command: run --unstable --allow-net --inspect=0.0.0.0:60220 /usr/src/deno_service/main.ts
    ports:
      - 64840:64840 # app service port
      - 60220:60220 # debugger port
    restart: "no"
```

This activates the debugger service that `deno` supports (Chrome Debugging Protocol). The debugger listens for commands on port `60220`.
This port is mapped to the same port on host machine, so VSCode debugger can connect to it.

When you start the debugger, it attaches to the running deno process, and sends to the debugger server commands.
