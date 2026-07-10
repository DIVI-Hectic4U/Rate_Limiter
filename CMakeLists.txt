cmake_minimum_required(VERSION 3.20)

project(
    RateLimiter
    VERSION 1.0.0
    DESCRIPTION "A production-style rate limiter for C++"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(
    RateLimiter
    ${SOURCES}
)

