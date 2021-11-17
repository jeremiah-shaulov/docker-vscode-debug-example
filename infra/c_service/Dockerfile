# ------------------------------------------------------------------------------
# Base image to be used by both debug and production
# ------------------------------------------------------------------------------

FROM silkeh/clang:12 as base
RUN apt-get update && \
	apt-get install -y libev-dev

RUN addgroup --gid=1969 c_service_user && \
	adduser --uid=1969 --gid=1969 --shell /bin/false --system c_service_user

WORKDIR /usr/src/c_service
COPY ./src/c_service .
RUN clang -DNDEBUG -O2 main.c -lev -o /usr/bin/c_service && \
	rm -r /usr/src/c_service


# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM base as debug

# 1. Install lldb-server, that will be running together with the app.
RUN apt-get update && \
	apt-get install -y lldb

# 2. App source code will be copied to /usr/src/c_service for compilation.
#	Later i will delete it.
WORKDIR /usr/src/c_service

# 3. Copy app source code, and compile it.
#	Store the resulting binary at /usr/bin/c_service, and delete the source code.
COPY ./src/c_service .
RUN clang -O0 -g main.c -lev -o /usr/bin/c_service && \
	rm -r /usr/src/c_service


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM debian:bullseye

WORKDIR /usr/src/c_service
COPY --from=base /usr/bin/c_service /usr/bin/c_service
USER c_service_user
