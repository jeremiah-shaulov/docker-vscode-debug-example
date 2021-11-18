# This Dockerfile file is based on technique about how to build small docker images
# by Shane Utt: https://shaneutt.com/blog/rust-fast-small-docker-image-builds/


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
#	Store the resulting binary at /usr/bin/rust_service, and delete the source code and all the intermediate files.
COPY ./src/rust_service .
RUN cargo build && \
	mv target/debug/rust_service -t /usr/bin


# ------------------------------------------------------------------------------
# Build the app
# ------------------------------------------------------------------------------

FROM rust:1.56 as cargo-build

# 1. We're gonna use Alpine Linux, that uses musl-libc
RUN apt-get update && \
	apt-get install -y musl-tools && \
	rustup target add x86_64-unknown-linux-musl

# 2. App source code will be copied to /usr/src/rust_service for compilation.
#	Later i will delete it.
WORKDIR /usr/src/rust_service

# 3. To improve build time, we want to create stub app that will have our Cargo.toml file,
#	so it will have the same dependencies.
#	We want to download the dependencies on this stage.
COPY ./src/rust_service/Cargo.toml Cargo.toml
RUN mkdir src && \
	echo 'fn main() {println!("Image build incomplete")}' > src/main.rs && \
	RUSTFLAGS=-Clinker=musl-gcc cargo build --release --target=x86_64-unknown-linux-musl && \
	rm -f target/x86_64-unknown-linux-musl/release/deps/rust_service*

# 4. Copy app source code, and compile it.
#	Store the resulting binary at /usr/bin/rust_service, and delete the source code and all the intermediate files.
COPY ./src/rust_service .
RUN RUSTFLAGS=-Clinker=musl-gcc cargo build --release --target=x86_64-unknown-linux-musl && \
	mv target/x86_64-unknown-linux-musl/release/rust_service -t /usr/bin && \
	rm -rf /usr/src/rust_service


# ------------------------------------------------------------------------------
# Install the app to fresh slim image
# ------------------------------------------------------------------------------

FROM alpine:3.14

# 1. Create user and group for the service.
#	I use 1030 as user and group id.
#	I think giving different user ids to different services improves isolation.
RUN adduser -u 1030 -g 1030 -s /bin/false -D rust_service_user

# 2. Copy the binary that compiled on the previous stage to this new Alpine Linux image.
COPY --from=cargo-build /usr/bin/rust_service /usr/bin/rust_service
RUN chown rust_service_user: /usr/bin/rust_service

USER rust_service_user
