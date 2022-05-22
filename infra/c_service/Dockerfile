# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM silkeh/clang:12 as debug

# 1. Create user for the service.
RUN addgroup --gid=1969 c_service_user && \
	adduser --uid=1969 --gid=1969 --shell /bin/false --system c_service_user

# 1. Install libev-dev
RUN apt-get update && \
	apt-get install -y --no-install-recommends libev-dev && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/*

# 2. App source code will be copied to /usr/src/c_service for compilation.
#	Later i will delete it.
WORKDIR /usr/src/c_service

# 3. Copy app source code
COPY ./src/c_service .

# 4. Compile 2 versions: debug and release.
#	Store the debug binary at /usr/bin/c_service, and the release binary at /usr/bin/c_service_release.
#	Then delete the source code.
RUN clang -O0 -g main.c -lev -o /usr/bin/c_service && \
	clang -DNDEBUG -O2 main.c -lev -o /usr/bin/c_service_release && \
	rm -r /usr/src/c_service

# 5. How to run the service
WORKDIR /home/c_service_user
CMD ["bash", "-c", "lldb-server platform --server --listen 0.0.0.0:2201 --gdbserver-port 9850 & /usr/bin/c_service"]

# app service port = 8543
# lldb-server listens for connections = 2201
# lldb-server service port = 9850
EXPOSE 8543 2201 9850


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM debian:bullseye-slim

RUN addgroup --gid=1969 c_service_user && \
	adduser --uid=1969 --gid=1969 --shell /bin/false --system c_service_user

RUN apt-get update && \
	apt-get install -y --no-install-recommends libev4 && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/*

COPY --from=debug --chown=root:root /usr/bin/c_service_release /usr/bin/c_service

WORKDIR /home/c_service_user
USER c_service_user
CMD ["/usr/bin/c_service"]

# app service port = 8543
EXPOSE 8543
