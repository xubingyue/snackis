cmake_minimum_required(VERSION 3.0)
project(snackis)
add_compile_options(-std=c++17 -Wall -Werror -g)

file(GLOB_RECURSE sources src/snackis/*.cpp)

add_executable(snackis ${sources} src/main.cpp)
target_include_directories(snackis PUBLIC src/)

find_package(Boost REQUIRED)

find_library(CURSES NAMES libcurses.a)
target_include_directories(snackis PUBLIC ${CURSES_INCLUDE_DIR})

find_library(CURL NAMES libcurl.a)
target_include_directories(snackis PUBLIC ${CURL_INCLUDE_DIR})

find_library(SODIUM NAMES libsodium.a)
target_include_directories(snackis PUBLIC ${SODIUM_INCLUDE_DIR})

target_link_libraries(snackis boost_system boost_filesystem curl sodium curses)
