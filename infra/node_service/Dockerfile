# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM node:17.1.0-alpine3.14 as debug

WORKDIR  /usr/src/node_service

# Cache dependencies
COPY --chown=root:root ./src/node_service/package.json ./package.json
RUN npm install

COPY --chown=root:root ./src/node_service .

WORKDIR /usr/src/node_service
USER node
CMD ["node", "--inspect=0.0.0.0:8548", "main.js"]

# app service port = 12982
# debugger port = 8548
EXPOSE 12982 8548


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM debug

WORKDIR /usr/src/node_service
USER node
CMD ["node", "main.js"]

# app service port = 12982
EXPOSE 12982
