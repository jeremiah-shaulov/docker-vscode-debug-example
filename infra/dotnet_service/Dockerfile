# ------------------------------------------------------------------------------
# Build the app
# ------------------------------------------------------------------------------

FROM mcr.microsoft.com/dotnet/sdk:6.0 as debug

RUN apt-get update && \
	apt-get install -y unzip procps && \
	curl -sSL https://aka.ms/getvsdbgsh | /bin/sh /dev/stdin -v latest -l /usr/bin/vsdbg

WORKDIR /usr/src/dotnet_service

COPY ./src/dotnet_service/dotnet_service.csproj ./dotnet_service.csproj
RUN dotnet restore

COPY ./src/dotnet_service .
RUN mkdir /usr/bin/dotnet_service && \
	dotnet publish --configuration Release --output /usr/bin/dotnet_service


# ------------------------------------------------------------------------------
# Install the app to fresh slim image
# ------------------------------------------------------------------------------

FROM mcr.microsoft.com/dotnet/runtime:6.0

RUN addgroup --gid=1654 dotnet_service_user && \
	adduser --uid=1654 --gid=1654 --shell /bin/false --system dotnet_service_user

COPY --from=debug /usr/bin/dotnet_service /usr/bin/dotnet_service

USER dotnet_service_user
