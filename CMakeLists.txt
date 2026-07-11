cmake_minimum_required(VERSION 3.20)

project(
    RateLimiter
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Toggle Redis
option(ENABLE_REDIS "Enable Redis distributed rate limiter" OFF)

find_package(Crow CONFIG REQUIRED)
if(ENABLE_REDIS)
    find_package(redis++ CONFIG REQUIRED)
    find_package(hiredis CONFIG REQUIRED)
endif()

# Source Dynamic List
set(SOURCES
    src/main.cpp
    src/http/HttpServer.cpp
    src/algorithms/FixedWindowLimiter.cpp
    src/algorithms/SlidingWindowLimiter.cpp
    src/algorithms/SlidingWindowCounterLimiter.cpp
    src/algorithms/TokenBucketLimiter.cpp
    src/algorithms/LeakyBucketLimiter.cpp
)

if(ENABLE_REDIS)
    list(APPEND SOURCES src/algorithms/RedisFixedWindowLimiter.cpp)
endif()

#Explicitly added list for reliable build
add_executable(
    RateLimiter
    ${SOURCES}
)

target_include_directories(RateLimiter PRIVATE include)

target_link_libraries(
    RateLimiter
    PRIVATE Crow::Crow 
)

if(ENABLE_REDIS)
    target_link_libraries(RateLimiter PRIVATE redis++::redis hiredis::hiredis)
    target_compile_definitions(RateLimiter PRIVATE ENABLE_REDIS)
endif()

