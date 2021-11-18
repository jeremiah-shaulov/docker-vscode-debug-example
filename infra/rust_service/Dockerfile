# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM rust:1.56 as debug

# 1. Install lldb-server, that will be running together with the app.
RUN apt-get update && \
	apt-get install -y lldb

# 2. App source code will be copied to /usr/src/rust_service for compilation.
#	Later i will delete it.
WORKDIR /usr/src/rust_service

# 3. To improve build time, we want to create stub app that will have our Cargo.toml file,
#	so it will have the same dependencies.
#	We want to download the dependencies on this stage.
COPY ./src/rust_service/Cargo.toml Cargo.toml
RUN mkdir src && \
	echo 'fn main() {println!("Image build incomplete")}' > src/main.rs && \
	cargo build && \
	rm -f target/debug/deps/rust_service*

# 4. Copy app source code, and compile it.
#	Store the resulting binary at /usr/bin/rust_service.
COPY ./src/rust_service .
RUN cargo build && \
	mv target/debug/rust_service -t /usr/bin


# ------------------------------------------------------------------------------
# Build the app for production
# ------------------------------------------------------------------------------

FROM debug as prod-build

WORKDIR /usr/src/rust_service
RUN cargo build --release && \
	mv target/release/rust_service -t /usr/bin && \
	rm -rf /usr/src/rust_service


# ------------------------------------------------------------------------------
# Install the app to fresh slim image
# ------------------------------------------------------------------------------

FROM debian:bullseye-slim

# 1. Create user and group for the service.
#	I use 1030 as user and group id.
#	I think giving different user ids to different services improves isolation.
RUN addgroup --gid=1030 rust_service_user && \
	adduser --uid=1030 --gid=1030 --shell /bin/false --system rust_service_user

# 2. Copy the binary that compiled on the previous stage to this new Debian image.
COPY --from=prod-build /usr/bin/rust_service /usr/bin/rust_service
RUN chown rust_service_user: /usr/bin/rust_service

USER rust_service_user
