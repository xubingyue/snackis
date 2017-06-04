cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_COMPILER clang++)
project(snackis)

add_compile_options(-std=c++1z -stdlib=libc++ -Wall -Werror -g)
set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")

file(GLOB_RECURSE sources src/snackis/*.cpp)
find_library(CURL NAMES libcurl.a)
find_library(FMT NAMES libfmt.a)
find_library(SODIUM NAMES libsodium.a)
find_library(UUID NAMES libuuid.a)

add_executable(tests ${sources} src/tests.cpp)
target_include_directories(tests PUBLIC src/ ${CURL_INCLUDE_DIR} ${FMT_INCLUDE_DIR} ${SODIUM_INCLUDE_DIR} ${UUID_INCLUDE_DIR})
target_link_libraries(tests c++experimental curl fmt sodium uuid)

file(GLOB_RECURSE ui_sources src/ui/*.cpp)
find_library(CURSES NAMES libcurses.a)
find_library(FORM NAMES libform.a)

add_executable(snackis ${sources} ${ui_sources} src/main.cpp)
target_include_directories(snackis PUBLIC src/ ${CURL_INCLUDE_DIR} ${FMT_INCLUDE_DIR} ${SODIUM_INCLUDE_DIR} ${UUID_INCLUDE_DIR} ${CURSES_INCLUDE_DIR} ${FORM_INCLUDE_DIR})
target_link_libraries(snackis c++experimental curl fmt sodium uuid curses form)
