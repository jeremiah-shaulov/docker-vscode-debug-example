# Docker VSCode remote debug example

This project demonstrates Docker infrastructure with services implemented in various languages, and how to debug these services from host machine using VSCode IDE.

This project shows services implemented in Deno, Rust, Swift, PHP, C#, Node JS, Python and C. Each service is simple TCP echo server.

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

3. Start the Docker services. They can be started either in dev or production mode.
Debugging will only work in dev. To start dev, `cd` to the directory where you cloned the project, and:

```bash
HTTP_PORT=8888 docker-compose -f docker-compose.yaml -f docker-compose.dev.yaml up -d --build --remove-orphans
```

Change HTTP_PORT if needed.

This starts 9 docker services:
- http_service
- php_fpm_service
- deno_service
- node_service
- rust_service
- swift_service
- c_service
- dotnet_service
- python_service

You can see the services state by executing:

```bash
docker ps --all
```

The main interface is `http_service`. It forwards requests to `php_fpm_service` through FastCGI protocol.

`php_fpm_service` has 1 web page called `index.php`. This script queries all the other services, and shows the result.

4. So open `http://localhost:8888/` in your browser, while services are running.

5. This project ships VSCode configuration file called `.vscode/launch.json`, that contains debug configurations for each one of the services.
If you switch to "Run and Debug" tab (Ctrl+Shift+D), you'll see all the configurations in the launch menu.

If you want to debug certain service, say `deno_service`, go to it's source file - `src/deno_service/main.ts`, find line that prints "Conn" message -
the program execution flow will hit this line each time a new connection to this service establishes.
Put a breakpoint at this line.
In launch tab of VSCode, select configuration called `deno_service: Attach to Docker`.
Refresh the `http://localhost:8888/` page.
The debugger must stop at that breakpoint.

6. To stop all the services do:

```bash
# from the project directory
docker-compose stop
```

Then you may want to remove the containers:

```bash
docker container prune
```
