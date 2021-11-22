# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM swift:5.5.1 as debug

# 1. App source code will be copied to /usr/src/swift_service for compilation.
WORKDIR /usr/src/swift_service

# 2. Cache dependencies.
COPY ./src/swift_service/Package.swift ./
RUN swift package resolve

# 3. Copy app source code.
COPY ./src/swift_service .

# 4. Compile 2 versions: debug and release.
#	Store the debug binary at /usr/bin/swift_service, and the release binary at /usr/bin/swift_service_release.
#	Then delete intemediate build files.
RUN swift build -Xswiftc -g && \
	swift build -c release && \
	mv .build/debug/swift_service /usr/bin/swift_service && \
	mv .build/release/swift_service /usr/bin/swift_service_release && \
	rm -rf .build

# 5. Run the app + lldb-server in background.
CMD ["bash", "-c", "lldb-server platform --server --listen 0.0.0.0:2418 --gdbserver-port 16276 & /usr/bin/swift_service"]

# app service port
EXPOSE 15880
# lldb-server listens for connections
EXPOSE 2418
# lldb-server service port
EXPOSE 16276


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM swift:5.5.1-slim

# 1. Create user for the service
RUN addgroup --gid=1355 swift_service_user && \
	adduser --uid=1355 --gid=1355 --shell /bin/false --system swift_service_user

# 2. App source code will be copied to /usr/src/swift_service for compilation.
#	Later i'll delete it.
WORKDIR /usr/src/swift_service

# 3. Copy the binary compiled in debug image.
COPY --from=debug /usr/bin/swift_service_release /usr/bin/swift_service

# 4. How to run the app.
USER swift_service_user
CMD ["/usr/bin/swift_service"]

# app service port
EXPOSE 15880
