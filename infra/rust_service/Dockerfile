# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM rust:1.56 as debug

# 1. Create user and group for the service.
#	I use 1030 as user and group id.
#	I think giving different user ids to different services improves isolation.
RUN addgroup --gid=1030 rust_service_user && \
	adduser --uid=1030 --gid=1030 --shell /bin/false --system rust_service_user

# 2. Install lldb-server, that will be running together with the app.
RUN apt-get update && \
	apt-get install -y --no-install-recommends lldb && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/*

# 3. App source code will be copied to /usr/src/rust_service for compilation.
#	Later i will delete it.
WORKDIR /usr/src/rust_service

# 4. To improve build time, we want to create stub app that will have our Cargo.toml file,
#	so it will have the same dependencies.
#	We want to download the dependencies on this stage.
COPY ./src/rust_service/Cargo.toml Cargo.toml
RUN mkdir src && \
	echo 'fn main() {println!("Image build incomplete")}' > src/main.rs && \
	cargo build && \
	rm -f target/debug/deps/rust_service*

# 5. Copy app source code.
COPY ./src/rust_service .

# 6. Compile 2 versions: debug and release.
#	Store the debug binary at /usr/bin/rust_service, and the release binary at /usr/bin/rust_service_release.
#	Then delete the source code and intemediate build files.
RUN cargo build && \
	mv target/debug/rust_service /usr/bin/rust_service && \
	cargo build --release && \
	mv target/release/rust_service /usr/bin/rust_service_release && \
	rm -rf /usr/src/rust_service

# 7. Run the app + lldb-server in background.
WORKDIR /home/rust_service_user
CMD ["bash", "-c", "lldb-server platform --server --listen 0.0.0.0:29935 --gdbserver-port 18088 & /usr/bin/rust_service"]

# app service port = 9023
# lldb-server listens for connections = 29935
# lldb-server service port = 18088
EXPOSE 9023 29935 18088


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
COPY --from=debug --chown=root:root /usr/bin/rust_service_release /usr/bin/rust_service

# 3. How to run the app.
WORKDIR /home/rust_service_user
USER rust_service_user
CMD ["/usr/bin/rust_service"]

# app service port = 9023
EXPOSE 9023
