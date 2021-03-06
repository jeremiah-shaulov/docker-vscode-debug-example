# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM swift:5.5.1 as debug

# 1. Create user for the service
RUN addgroup --gid=1355 swift_service_user && \
	adduser --uid=1355 --gid=1355 --shell /bin/false --system swift_service_user

# 2. App source code will be copied to /usr/src/swift_service for compilation.
WORKDIR /usr/src/swift_service

# 3. Cache dependencies.
COPY ./src/swift_service/Package.swift ./
RUN swift package resolve

# 4. Copy app source code.
COPY ./src/swift_service .

# 5. Compile 2 versions: debug and release.
#	Store the debug binary at /usr/bin/swift_service, and the release binary at /usr/bin/swift_service_release.
#	Then delete the source code and intemediate build files.
RUN swift build -Xswiftc -g && \
	mv .build/debug/swift_service /usr/bin/swift_service && \
	swift build -c release && \
	mv .build/release/swift_service /usr/bin/swift_service_release && \
	rm -rf /usr/src/swift_service

# 6. Run the app + lldb-server in background.
WORKDIR /home/swift_service_user
CMD ["bash", "-c", "lldb-server platform --server --listen 0.0.0.0:2418 --gdbserver-port 16276 & /usr/bin/swift_service"]

# app service port = 15880
# lldb-server listens for connections = 2418
# lldb-server service port = 16276
EXPOSE 15880 2418 16276


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM swift:5.5.1-slim

# 1. Create user for the service
RUN addgroup --gid=1355 swift_service_user && \
	adduser --uid=1355 --gid=1355 --shell /bin/false --system swift_service_user

# 2. Copy the binary compiled in debug image.
COPY --from=debug --chown=root:root /usr/bin/swift_service_release /usr/bin/swift_service

# 3. How to run the app.
WORKDIR /home/swift_service_user
USER swift_service_user
CMD ["/usr/bin/swift_service"]

# app service port = 15880
EXPOSE 15880
