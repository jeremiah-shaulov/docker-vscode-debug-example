# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM swift:5.5 as debug

# 1. Install lldb-server, that will be running together with the app.
RUN apt-get update && \
	apt-get install -y lldb

# 2. App source code will be copied to /usr/src/swift_service for compilation.
WORKDIR /usr/src/swift_service

# 3. To improve build time, we want to create stub app that will have our Package.swift file,
#	so it will have the same dependencies.
#	We want to download the dependencies on this stage.
COPY ./src/swift_service/Package.swift ./
RUN swift package resolve

# 4. Copy app source code, and compile it.
#	Store the resulting binary at /usr/bin/swift_service.
COPY ./src/swift_service .
RUN swift build -c release -Xswiftc -g && \
	mv .build/x86_64-unknown-linux-gnu/release/swift_service -t /usr/bin


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM swift:5.5

RUN addgroup --gid=1355 swift_service_user && \
	adduser --uid=1355 --gid=1355 --shell /bin/false --system swift_service_user

WORKDIR /usr/src/swift_service

COPY ./src/swift_service/Package.swift ./
RUN swift package resolve

COPY ./src/swift_service .
RUN swift build -c release -Xswiftc -g && \
	mv .build/x86_64-unknown-linux-gnu/release/swift_service -t /usr/bin && \
	rm -rf /usr/src/swift_service

USER swift_service_user
