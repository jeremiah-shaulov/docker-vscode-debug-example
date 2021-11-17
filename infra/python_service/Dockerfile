# ------------------------------------------------------------------------------
# Base image to be used by both debug and production
# ------------------------------------------------------------------------------

FROM python:3.10-alpine3.14 as base

RUN adduser -u 1675 -g 1675 -s /bin/false -D python_service_user

WORKDIR /usr/src/python_service
USER python_service_user

COPY ./src/python_service/requirements.txt ./requirements.txt
RUN pip install -r requirements.txt


# ------------------------------------------------------------------------------
# Debug image
# ------------------------------------------------------------------------------

FROM base as debug

# Install Python debugger for Vscode
RUN pip install ptvsd

WORKDIR /usr/src/python_service
USER python_service_user

COPY ./src/python_service .


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM base

WORKDIR /usr/src/python_service
USER python_service_user

COPY ./src/python_service .
