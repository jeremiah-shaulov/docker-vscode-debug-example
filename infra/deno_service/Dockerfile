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

# 2. Source code will be copied to here.
WORKDIR /usr/src/deno_service

# 3. Copy the app
COPY --chown=root:root ./src/deno_service .

# 4. On first run deno checks source files. I want to check them only once at build time.
USER deno
RUN deno cache --unstable main.ts && \
	find . -name '*.test.ts' | xargs --no-run-if-empty deno cache --unstable

CMD ["run", "--unstable", "--allow-net", "--inspect=0.0.0.0:48050", "main.ts"]

# app service port
EXPOSE 5090
# debugger port
EXPOSE 48050


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM denoland/deno:distroless-1.16.2

COPY --from=debug --chown=1993 /deno-dir /deno-dir
COPY --from=debug --chown=root:root /usr/src/deno_service /usr/src/deno_service

WORKDIR /usr/src/deno_service
USER 1993
CMD ["run", "--unstable", "--allow-net", "main.ts"]

# app service port
EXPOSE 5090
