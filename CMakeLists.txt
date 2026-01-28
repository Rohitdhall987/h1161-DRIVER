cmake_minimum_required(VERSION 4.1)
project(h1161-driver VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED QUIET)
pkg_search_module(UDEV REQUIRED libudev)

set(SOURCES
        src/main.cpp
        src/utils.cpp
        src/hid_reader.cpp
        src/hid_monitor.cpp
        src/tablet_parser.cpp
)

add_executable(h1161-driver ${SOURCES})

target_link_libraries(h1161-driver PRIVATE ${UDEV_LIBRARIES})
target_include_directories(h1161-driver PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${UDEV_INCLUDE_DIRS}
)

target_compile_definitions(h1161-driver PRIVATE RELEASE=0)

target_compile_options(h1161-driver PRIVATE -Wall -Wextra -Wpedantic)
