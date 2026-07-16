FROM ubuntu:24.04

#Prevent interactive timezone prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# 1. Install system build dependencies and Redis C++ clients
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libasio-dev \
    libhiredis-dev \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# 2. Intall Crow Framework from source
RUN git clone https://github.com/CrowCpp/Crow.git /tmp/Crow \
    && cd /tmp/Crow \
    && mkdir build && cd build \
    && cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF \
    && make install \
    && rm -rf /tmp/Crow 

RUN git clone https://github.com/sewenew/redis-plus-plus.git /tmp/redis-plus-plus \
    && cd /tmp/redis-plus-plus \
    && mkdir build && cd build \
    && cmake .. -DREDIS_PLUS_PLUS_BUILD_TEST=OFF -DREDIS_PLUS_PLUS_BUILD_ASYNC=OFF \
    && make install \
    && rm -rf /tmp/redis-plus-plus

# 3. Setup application directory
WORKDIR /app
COPY . .

# 4. Compile the C++ application with Redis Explicitly enabled !
RUN mkdir -p build && cd build \
    && cmake .. -DENABLE_REDIS=ON \
    && make

# Expose the API PORT
EXPOSE 8080

# Start the Server
CMD ["./build/RateLimiter"]
