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

CMD ["python", "-m", "ptvsd", "--host", "0.0.0.0", "--port", "22742", "--multiprocess", "main.py"]

# app service port
EXPOSE 8497
# debugger port
EXPOSE 22742


# ------------------------------------------------------------------------------
# Production image
# ------------------------------------------------------------------------------

FROM base

WORKDIR /usr/src/python_service
USER python_service_user

COPY ./src/python_service .

CMD ["python", "main.py"]

# app service port
EXPOSE 8497
