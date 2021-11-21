# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM swift:5.5 as debug

# 1. App source code will be copied to /usr/src/swift_service for compilation.
WORKDIR /usr/src/swift_service

# 2. Cache dependencies.
COPY ./src/swift_service/Package.swift ./
RUN swift package resolve

# 3. Copy app source code, and compile it.
#	Store the resulting binary at /usr/bin/swift_service.
COPY ./src/swift_service .
RUN swift build -c release -Xswiftc -g && \
	mv .build/x86_64-unknown-linux-gnu/release/swift_service -t /usr/bin

# 4. Run the app + lldb-server in background.
CMD ["bash", "-c", "lldb-server platform --server --listen 0.0.0.0:56439 --gdbserver-port 16276 & swift run"]

# app service port
EXPOSE 15880
# lldb-server listens for connections
EXPOSE 56439
# lldb-server service port
EXPOSE 16276


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM swift:5.5

# 1. Create user for the service
RUN addgroup --gid=1355 swift_service_user && \
	adduser --uid=1355 --gid=1355 --shell /bin/false --system swift_service_user

# 2. App source code will be copied to /usr/src/swift_service for compilation.
#	Later i'll delete it.
WORKDIR /usr/src/swift_service

# 3. Cache dependencies.
COPY ./src/swift_service/Package.swift ./
RUN swift package resolve

# 4. Copy app source code, and compile it.
#	Store the resulting binary at /usr/bin/swift_service.
#	Then delete the source code.
COPY ./src/swift_service .
RUN swift build -c release -Xswiftc -g && \
	mv .build/x86_64-unknown-linux-gnu/release/swift_service -t /usr/bin && \
	rm -rf /usr/src/swift_service

# 5. How to run the app.
USER swift_service_user
CMD ["/usr/bin/swift_service"]

# app service port
EXPOSE 15880
