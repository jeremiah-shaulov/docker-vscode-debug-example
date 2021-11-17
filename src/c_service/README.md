# TCP echo service with C

Rock'n'roll, stylish haircuts, cigarette smoke, and C programming language - this is the magic of 1970s.
Is this joy or pain to create Docker services in C, or maybe both, i don't know, but here is the example of how this can be achieved.

This directory of the [repository](../../Readme.md) contains C implementation of simple Docker service.
This service can comminicate with other services, and it's possible to debug it from host machine with VSCode IDE.
The debugger can be attached to the process running in the container, and you can stop on breakpoints.

See main [page](../../Readme.md) on how to run this project. Here i'll explain what the debugging process looks like.

In [Dockerfile](../../infra/c_service/Dockerfile) in the section for dev image, i install `lldb`:

```bash
apt-get install -y lldb
```

This package includes `lldb-server` that is running in parallel with the application. I run it like this from [docker-compose.dev.yaml](../docker-compose.dev.yaml):

```dockerfile
c_service:
    build:
      target: debug
    command: bash -c "lldb-server platform --server --listen 0.0.0.0:54255 --gdbserver-port 9850 & /usr/bin/c_service"
    pid: host
    ports:
      - 8543:8543 # app service port
      - 54255:54255 # lldb-server listens for connections
      - 9850:9850 # lldb-server service port
    restart: "no"
```
The debugger listens on port `54255`. This port is also mapped to the host machine.
During communication with debugger client (on host machine), `lldb-server` uses another port - `9850`, and it must be mapped to the host as well.

Notice `pid: host` line. This tells docker to share process id namespace with host. With this flag set, host can see processes of the container in it's `/proc` or `ps aux`.
And also container will see host processes.

Then in [launch.json](../.vscode/launch.json) the configuration looks like this:

```json
"configurations":
[	{	"name": "c_service: Attach to Docker",
		"type": "lldb",
		"request": "attach",
		"pid": "${input:c_service_pid}",
		"initCommands":
		[	"platform select remote-linux",
			"platform connect connect://localhost:54255",
			"settings set target.inherit-env false",
			"settings set target.source-map /usr/src/c_service ${workspaceFolder}/src/c_service"
		]
	}
],
"inputs":
[	{	"id": "c_service_pid",
		"type": "command",
		"command": "shellCommand.execute",
		"args":
		{	"command": "pidof c_service"
		}
	}
]
```

So VSCode will connect to `localhost:54255`, which is mapped to `<c_service in docker>:54255` to communicate with the debugger.

It's also possible to debug with `lldb` client installed on your machine, bypassing VSCode, or to run lldb from a Docker image:

```bash
# from project root directory
docker run -it --rm --network=host --pid=host -v "$PWD:$PWD" silkeh/clang:12 lldb -o 'platform select remote-linux' -o 'platform connect connect://localhost:54255' -o 'attach '(pidof c_service) -o "settings set target.source-map /usr/src/c_service $PWD/src/c_service"
```

Then inside the `lldb` client you can put a breakpoint on `handle_conn` function:

```
(lldb) b handle_conn
```

Then in parallel execute:
```bash
curl --output - 'http://l.personyze.com:8888/'
```

This must hang, because the execution is suspended on our `handle_conn` function. See this in `lldb`:

```
(lldb) l
   238                  return EXIT_FAILURE;
   239          }
   240
   241          // Bind
   242          struct sockaddr_in s;
   243          s.sin_family = AF_INET;
   244          s.sin_port = htons(PORT);
   245          s.sin_addr.s_addr = htonl(INADDR_ANY);
   246          int err = bind(server_fd, (struct sockaddr *)&s, sizeof(s));
   247          if (err == -1)
(lldb) r
```
