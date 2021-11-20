# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM node:17.1.0-alpine3.14 as debug

WORKDIR  /usr/src/node_service

# Cache dependencies
COPY ./src/node_service/package.json ./package.json
RUN npm install

COPY ./src/node_service .
RUN chown -R root:node . && \
	chmod -R 750 .

USER node
CMD ["node", "--inspect=0.0.0.0:8548", "/usr/src/node_service/main.js"]

# app service port
EXPOSE 12982
# debugger port
EXPOSE 8548


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM debug

USER node
CMD ["node", "/usr/src/node_service/main.js"]

# app service port
EXPOSE 12982
