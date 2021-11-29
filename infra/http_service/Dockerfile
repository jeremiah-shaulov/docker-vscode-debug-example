FROM nginx:1.21

# 1. Copy server conf
COPY ./infra/http_service/default.conf /etc/nginx/conf.d/default.conf

# 2. Copy files of php_fpm_service (pages)
COPY --chown=root:root ./src/php_fpm_service /usr/src/php_fpm_service

EXPOSE 80
