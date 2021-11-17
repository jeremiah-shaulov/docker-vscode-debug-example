FROM denoland/deno:1.15.3

# 1. To improve build time, copy deps.ts, and cache the app dependencies.
USER deno
COPY ./src/deno_service/private/deps.ts /tmp/deps.ts
RUN deno cache --unstable /tmp/deps.ts
USER root
RUN rm /tmp/deps.ts

# 2. Copy the app
COPY ./src/deno_service /usr/src/deno_service
RUN chown -R root:deno /usr/src/deno_service && \
	chmod -R 750 /usr/src/deno_service

USER deno
