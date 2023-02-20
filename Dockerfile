FROM redis:7.0.8
WORKDIR /usr/src/app
COPY redis.conf .
EXPOSE 6379
CMD ["redis-server", "redis.conf"]