cmake_minimum_required(VERSION 4.1)
project(h1161-driver VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        src/main.cpp
        src/utils.cpp
)

add_executable(h1161-driver ${SOURCES})

target_include_directories(h1161-driver PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(h1161-driver PRIVATE RELEASE=0)

target_compile_options(h1161-driver PRIVATE -Wall -Wextra -Wpedantic)
