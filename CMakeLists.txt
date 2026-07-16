cmake_minimum_required(VERSION 3.20)

project(
    RateLimiter
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_REDIS "Enable Redis distributed rate limiter" OFF)

# Find essential core libraries
find_package(Crow CONFIG REQUIRED)
find_package(Threads REQUIRED)

# --- BULLETPROOF LINUX LIBRARY LINKING ---
# Instead of relying on missing vendor config files, we grab the raw Linux binaries directly!
if(ENABLE_REDIS)
    find_path(REDIS_PLUS_PLUS_INCLUDE_DIR NAMES sw/redis++/redis++.h)
    find_library(REDIS_PLUS_PLUS_LIB NAMES redis++)
    find_library(HIREDIS_LIB NAMES hiredis)
    
    if(NOT REDIS_PLUS_PLUS_INCLUDE_DIR OR NOT REDIS_PLUS_PLUS_LIB OR NOT HIREDIS_LIB)
        message(FATAL_ERROR "Redis libraries not found! Ensure libhiredis-dev and libredis++-dev are installed.")
    endif()
endif()

# --- SOURCE FILES ---
set(SOURCES
    src/main.cpp
    src/http/HttpServer.cpp
    src/core/RateLimiterFactory.cpp
    src/algorithms/FixedWindowLimiter.cpp
    src/algorithms/SlidingWindowLogLimiter.cpp
    src/algorithms/SlidingWindowCounterLimiter.cpp
    src/algorithms/TokenBucketLimiter.cpp
    src/algorithms/LeakyBucketLimiter.cpp
)

if(ENABLE_REDIS)
    list(APPEND SOURCES src/algorithms/RedisFixedWindowLimiter.cpp)
endif()

add_executable(RateLimiter ${SOURCES})

target_include_directories(RateLimiter PRIVATE include)

target_link_libraries(
    RateLimiter
    PRIVATE 
        Crow::Crow
        Threads::Threads
)

if(ENABLE_REDIS)
    target_include_directories(RateLimiter PRIVATE ${REDIS_PLUS_PLUS_INCLUDE_DIR})
    target_link_libraries(RateLimiter PRIVATE ${REDIS_PLUS_PLUS_LIB} ${HIREDIS_LIB})
    target_compile_definitions(RateLimiter PRIVATE ENABLE_REDIS)
endif()