FROM node:17.1.0-alpine3.14

WORKDIR  /usr/src/node_service

COPY ./src/node_service/package.json ./package.json
RUN npm install

COPY ./src/node_service .
RUN chown -R root:node . && \
	chmod -R 750 .

USER node
