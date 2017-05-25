cmake_minimum_required(VERSION 3.0)
project(snackis)
add_compile_options(-std=c++17 -Wall -Werror -g)
file(GLOB_RECURSE sources src/snackis/*.cpp)
find_package(Boost REQUIRED)
find_library(CURL NAMES libcurl.a)
find_library(SODIUM NAMES libsodium.a)

add_executable(tests ${sources} src/tests.cpp)
target_include_directories(tests PUBLIC src/ ${CURL_INCLUDE_DIR} ${SODIUM_INCLUDE_DIR})
target_link_libraries(tests boost_system boost_thread boost_filesystem curl sodium)

file(GLOB_RECURSE ui_sources src/ui/*.cpp)
find_library(CURSES NAMES libcurses.a)
find_library(FORM NAMES libform.a)

add_executable(snackis ${sources} ${ui_sources} src/main.cpp)
target_include_directories(snackis PUBLIC src/ ${CURL_INCLUDE_DIR} ${SODIUM_INCLUDE_DIR} ${CURSES_INCLUDE_DIR} ${FORM_INCLUDE_DIR})
target_link_libraries(snackis boost_system boost_thread boost_filesystem curl sodium curses form)
