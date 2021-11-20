# ------------------------------------------------------------------------------
# Build the app
# ------------------------------------------------------------------------------

FROM mcr.microsoft.com/dotnet/sdk:6.0 as debug

# Install vsdbg (dotnet debugger).
RUN apt-get update && \
	apt-get install -y --no-install-recommends unzip && \
	rm -rf /var/lib/apt/lists/* && \
	curl -sSL https://aka.ms/getvsdbgsh | /bin/sh /dev/stdin -v latest -l /usr/bin/vsdbg

WORKDIR /usr/src/dotnet_service

# Cache dependencies
COPY ./src/dotnet_service/dotnet_service.csproj ./dotnet_service.csproj
RUN dotnet restore

# Release build that will not be used in this image, but will be copied to the production image.
COPY ./src/dotnet_service .
RUN mkdir /usr/bin/dotnet_service && \
	dotnet publish --configuration Release --output /usr/bin/dotnet_service

CMD ["dotnet", "run"]

# app service port
EXPOSE 7287


# ------------------------------------------------------------------------------
# Install the app to fresh slim image
# ------------------------------------------------------------------------------

FROM mcr.microsoft.com/dotnet/runtime:6.0

# Create user for the service
RUN addgroup --gid=1654 dotnet_service_user && \
	adduser --uid=1654 --gid=1654 --shell /bin/false --system dotnet_service_user

# Previous image already compiled release version for me, so copy it
COPY --from=debug /usr/bin/dotnet_service /usr/bin/dotnet_service

USER dotnet_service_user
CMD ["dotnet", "/usr/bin/dotnet_service/dotnet_service.dll"]

# app service port
EXPOSE 7287
