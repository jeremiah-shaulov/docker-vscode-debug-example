FROM nginx:1.21

# 1. Copy server conf
COPY ./infra/http_service/default.conf /etc/nginx/conf.d/default.conf

# 2. Copy files of php_fpm_service (pages)
COPY ./src/php_fpm_service /usr/src/php_fpm_service
RUN chown -R root:root /usr/src/php_fpm_service && \
	chmod -R 755 /usr/src/php_fpm_service

EXPOSE 80
