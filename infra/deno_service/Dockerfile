# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM denoland/deno:debian-1.19.3 as debug

# 1. Source code will be copied to here.
WORKDIR /usr/src/deno_service

# 2. Run from this user.
USER deno

# 3. To improve build time, copy deps.ts, and cache the app dependencies.
COPY --chown=root:root ./src/deno_service/private/deps.ts ./private/deps.ts
RUN deno cache --unstable ./private/deps.ts

# 4. Copy the app and cache it. Also cache all *.test.ts files if such are found.
COPY --chown=root:root ./src/deno_service .
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

FROM denoland/deno:distroless-1.19.3

WORKDIR /usr/src/deno_service

COPY --from=debug --chown=1993 /deno-dir /deno-dir
COPY --from=debug --chown=root:root /usr/src/deno_service .

USER 1993
CMD ["run", "--unstable", "--allow-net", "main.ts"]

# app service port
EXPOSE 5090
