# ------------------------------------------------------------------------------
# Base image to be used by both debug and production
# ------------------------------------------------------------------------------

FROM php:8.0-fpm as base

# 1. Install required extensions
#	Here i install pdo_mysql (for demonstration purposes, i'll not be using it)
RUN docker-php-ext-install pdo pdo_mysql

# 2. Install composer
COPY --from=composer:2.1 /usr/bin/composer /usr/bin/composer
# Composer wants `unzip`
RUN apt-get update && \
	apt-get install -y --no-install-recommends unzip && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/*

# 3. Create user for this service
RUN groupadd -g 1000 php_fpm_service_user && \
	useradd -u 1000 -g php_fpm_service_user -s /bin/false php_fpm_service_user

# 4. Delete default config, and don't use default port 9000.
#	And and use php.ini-production.
RUN rm /usr/local/etc/php-fpm.d/www.conf.default && \
	sed --regexp-extended -e 's/listen\s*=\s*9000//g' -i /usr/local/etc/php-fpm.d/zz-docker.conf && \
	cd /usr/local/etc/php && \
	ln -s php.ini-production php.ini

# 5. Copy my version of www.conf
COPY ./infra/php_fpm_service/www.conf /usr/local/etc/php-fpm.d/www.conf


# ------------------------------------------------------------------------------
# Debug image to target
# ------------------------------------------------------------------------------

FROM base as debug

# 1. xdebug
RUN pecl install xdebug && \
	docker-php-ext-enable xdebug
COPY ./infra/php_fpm_service/xdebug.ini /usr/local/etc/php/conf.d/docker-php-ext-xdebug.ini

# 2. Workdir
WORKDIR /usr/src/php_fpm_service

# 3. Install dependencies
COPY --chown=root:php_fpm_service_user ./src/php_fpm_service/composer.json ./composer.json
RUN composer update

# 4. Copy the app
COPY --chown=root:php_fpm_service_user ./src/php_fpm_service .

USER php_fpm_service_user
EXPOSE 21104


# ------------------------------------------------------------------------------
# Final production image
# ------------------------------------------------------------------------------

FROM base

# 1. Workdir
WORKDIR /usr/src/php_fpm_service

# 2. Install dependencies
COPY --chown=root:php_fpm_service_user ./src/php_fpm_service/composer.json ./composer.json
RUN composer update

# 3. Copy the app
COPY --chown=root:php_fpm_service_user ./src/php_fpm_service .

USER php_fpm_service_user
EXPOSE 21104
