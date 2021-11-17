# ------------------------------------------------------------------------------
# Base
# ------------------------------------------------------------------------------

FROM php:8.0-fpm as base

# 1. Install required extensions
#	Here i install pdo_mysql (for demonstration purposes, i'll not be using it)
RUN docker-php-ext-install pdo pdo_mysql

# 2. Create user for this service
RUN groupadd -g 1000 php_fpm_service_user && \
	useradd -u 1000 -g php_fpm_service_user -s /bin/false php_fpm_service_user

# 3. Delete default config
RUN rm /usr/local/etc/php-fpm.d/www.conf.default && \
	sed --regexp-extended -e 's/listen\s*=\s*9000//g' -i /usr/local/etc/php-fpm.d/zz-docker.conf

# 4. Copy my version of www.conf
COPY ./infra/php_fpm_service/www.conf /usr/local/etc/php-fpm.d/www.conf


# ------------------------------------------------------------------------------
# Debug image to target
# ------------------------------------------------------------------------------

FROM base as debug

# 1. xdebug
RUN pecl install xdebug && \
	docker-php-ext-enable xdebug
COPY ./infra/php_fpm_service/xdebug.ini /usr/local/etc/php/conf.d/docker-php-ext-xdebug.ini

# 2. Copy the app
COPY ./src/php_fpm_service /usr/src/php_fpm_service
RUN chown -R root:php_fpm_service_user /usr/src && \
	chmod -R 750 /usr/src

USER php_fpm_service_user


# ------------------------------------------------------------------------------
# Final production image
# ------------------------------------------------------------------------------

FROM base

# Copy the app
COPY ./src/php_fpm_service /usr/src/php_fpm_service
RUN chown -R root:php_fpm_service_user /usr/src && \
	chmod -R 750 /usr/src

USER php_fpm_service_user
