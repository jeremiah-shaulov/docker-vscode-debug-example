# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM denoland/deno:debian-1.16.2 as debug

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
CMD ["run", "--unstable", "--allow-net", "--inspect=0.0.0.0:60220", "/usr/src/deno_service/main.ts"]

# app service port
EXPOSE 64840
# debugger port
EXPOSE 60220


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM denoland/deno:distroless-1.16.2

COPY --from=debug --chown=1993 /deno-dir /deno-dir
COPY --from=debug --chown=1993 /usr/src/deno_service /usr/src/deno_service

USER 1993
CMD ["run", "--unstable", "--allow-net", "/usr/src/deno_service/main.ts"]

# app service port
EXPOSE 64840
