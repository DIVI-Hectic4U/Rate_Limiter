cmake_minimum_required(VERSION 3.20)

project(
    RateLimiter
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)

include_directories(include)

file(GLOB_RECURSE SOURCES 
    src/*.cpp
)

add_executable(
    RateLimiter
    ${SOURCES}
)

target_link_libraries(
    RateLimiter
    PRIVATE Crow::Crow
)

