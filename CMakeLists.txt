cmake_minimum_required(VERSION 3.20)

project(
    RateLimiter
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)
find_package(hiredis CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

#Explicitly added list for reliable build
add_executable(
    RateLimiter
    src/main.cpp
    src/http/HttpServer.cpp
    src/algorithms/FixedWindowLimiter.cpp
    src/algorithms/SlidingWindowLimiter.cpp
    src/algorithms/SlidingWindowCounterLimiter.cpp
    src/algorithms/TokenBucketLimiter.cpp
    src/algorithms/LeakyBucketLimiter.cpp
    src/algorithms/RedisFixedWindowLimiter.cpp
)

target_include_directories(RateLimiter PRIVATE include)

target_link_libraries(
    RateLimiter
    PRIVATE 
        Crow::Crow
        redis++::redis++
        hiredis::hiredis
)

