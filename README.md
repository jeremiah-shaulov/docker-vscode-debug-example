# Docker VSCode remote debug example

This project demonstrates Docker infrastructure with services implemented in various languages, and how to debug these services from host machine using VSCode IDE.

This project shows services implemented in Deno, Rust, Swift, PHP, C#, Node JS, Python, C and Java. Each service is a simple TCP echo server.

VSCode debugger can attach to any running service, you can put breakpoints, and inspect variables in runtime.

I tested all this on Kubuntu 21.10 host. Let me know if something doesn't work on your system.

1. To get started, first clone this repo:

```bash
git clone https://github.com/jeremiah-shaulov/docker-vscode-debug-example.git
```

2. I needed to make changes in my system configuration in order to be able to debug lldb-backed containers (Rust, Swift, C). This is what i needed to change:

```bash
cat /proc/sys/kernel/yama/ptrace_scope # see original value
echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
```

3. Debugging functionality in VSCode is provided by extensions. You'll need these:

- [C#](https://marketplace.visualstudio.com/items?itemName=ms-dotnettools.csharp)
- [Debugger for Java](https://marketplace.visualstudio.com/items?itemName=vscjava.vscode-java-debug)
- [PHP Debug](https://marketplace.visualstudio.com/items?itemName=felixfbecker.php-debug)
- [Python](https://marketplace.visualstudio.com/items?itemName=ms-python.python)
- [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb)

4. Start the Docker services. They can be started either in dev or production mode.
Debugging will only work in dev. To start dev, `cd` to the directory where you cloned the project, and:

```bash
HTTP_PORT=8888 docker-compose -f docker-compose.yaml -f docker-compose.dev.yaml up -d --build
```

Change HTTP_PORT if needed.

Every time you edit something either in infrastructure definition ([docker-compose.yaml](./docker-compose.yaml), `Dockerfile`) or in source code, you need to rerun the above command to rebuild the containers.

This starts 10 docker services:
- http_service
- php_fpm_service
- deno_service
- node_service
- rust_service
- swift_service
- c_service
- dotnet_service
- java_service
- python_service

You can see the services state by executing:

```bash
docker ps --all
```

If you want to start the same services in production mode (slimmer images, faster execution, and impossible debugging), then run `docker-compose up` like this:

```bash
HTTP_PORT=8888 docker-compose up -d --build
```

But in this article, i always assume that you're running in dev mode.

The main interface is `http_service`. It forwards requests to `php_fpm_service` through FastCGI protocol.

`php_fpm_service` has 1 web page called `index.php`. This script queries all the other services, and shows the result.

5. So open `http://localhost:8888/` in your browser, while services are running.

6. This project ships VSCode configuration file called [.vscode/launch.json](./.vscode/launch.json), that contains debug configurations for each one of the services.
If you switch to "Run and Debug" tab (Ctrl+Shift+D), you'll see all the configurations in the launch menu.

If you want to debug certain service, say `deno_service`, open it's source file in VSCode - `src/deno_service/main.ts`, find line that prints "Conn" message -
the program execution flow will hit this line each time a new connection to this service establishes.
Put a breakpoint at this line.
In launch tab of VSCode, select configuration called `deno_service: Attach to Docker`.
Refresh the `http://localhost:8888/` page.
The debugger must stop at that breakpoint.

7. To stop all the services and delete containers do:

```bash
# from the project directory
docker-compose down
```

## Services architecture

`http_service` listens for HTTP requests on port that you can override with `HTTP_PORT` environment variable when you run `docker-compose up` (in this article i use value 8888).
This service forwards requests to "*.php" files to "php_fpm_service:21104".

Within Docker infrastructure each container has hostname matching the service name, So from within any container, "http_service:8888" is the "host:port" where HTTP server is running.
And PHP FastCGI server is running on "php_fpm_service:21104". Port 21104 is hard-coded to various files in this project.

This project uses list of "well-known" ports for it's services.

- http_service - ${HTTP_PORT} (HTTP).
- php_fpm_service - 21104 (FastCGI), and it connects to "host.docker.internal:33078". Docker containers can reach host machine by the "host.docker.internal" hostname. php_fpm_service assumes that PHP debugger (XDebug) is listening on "host.docker.internal:33078". It will listen when you select corresponding debug configuration, and press F5.
- deno_service - 5090 (TCP), and 48050 (debugger server, Chrome Debugging Protocol). VSCode will connect to this port when you select corresponding debug configuration, and press F5.
- node_service - 12982 (TCP), and 8548 (debugger server, Chrome Debugging Protocol).
- rust_service - 9023 (TCP), and 2 ports for debugger server: 29935 and 18088 (lldb-server).
- swift_service - 15880 (TCP), and 2 ports for debugger server: 2418 and 16276 (lldb-server).
- c_service - 8543 (TCP), and 2 ports for debugger server: 2201 and 9850 (lldb-server).
- dotnet_service - 7287 (TCP), and it uses vsdbg debugger. VSCode knows to start the debugger within container, because the container name is assumed to be "docker-vscode-debug-example_dotnet_service_1", and is hardcoded in [.vscode/launch.json](./.vscode/launch.json).
- java_service - 27712 (TCP), 9455 (debugger, ptvsd)
- python_service - 8497 (TCP), 22742 (debugger, ptvsd)

All the port numbers are random (`Math.floor(Math.random() * 0xBFFF)`).

This project is intended to be a good starting point for other projects. If you base your project on this, you'll need to rename services, and ports in all files where they appear.

## About services

Now let's take a closer look to each one of the services.

- [php_fpm_service](./src/php_fpm_service)
- [deno_service](./src/deno_service)
- [node_service](./src/node_service)
- [rust_service](./src/rust_service)
- [swift_service](./src/swift_service)
- [c_service](./src/c_service)
- [dotnet_service](./src/dotnet_service)
- [java_service](./src/java_service)
- [python_service](./src/python_service)
