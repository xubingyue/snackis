cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_COMPILER clang++)
project(snackis)

add_compile_options(-std=c++1z -stdlib=libc++ -Wall -Werror -g)
set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")

file(GLOB_RECURSE core_src src/snackis/core/*.cpp)
file(GLOB_RECURSE crypt_src src/snackis/crypt/*.cpp)
file(GLOB_RECURSE db_src src/snackis/db/*.cpp)
file(GLOB_RECURSE net_src src/snackis/net/*.cpp)
file(GLOB snackis_src src/snackis/*.cpp)

find_library(CURL NAMES libcurl.a)
find_library(SODIUM NAMES libsodium.a)
find_library(UUID NAMES libuuid.a)

add_executable(tests ${core_src} ${crypt_src} ${db_src} ${net_src} ${snackis_src} src/tests.cpp)
target_include_directories(tests PUBLIC src/ ${CURL_INCLUDE_DIR} ${SODIUM_INCLUDE_DIR} ${UUID_INCLUDE_DIR})
target_link_libraries(tests c++experimental curl sodium uuid)

file(GLOB_RECURSE ui_src src/snackis/ui/*.cpp)
find_library(CURSES NAMES libcurses.a)
find_library(FORM NAMES libform.a)

add_executable(snackis ${core_src} ${crypt_src} ${db_src} ${net_src} ${snackis_src} ${ui_src} src/main.cpp)
target_include_directories(snackis PUBLIC src/ ${CURL_INCLUDE_DIR} ${SODIUM_INCLUDE_DIR} ${UUID_INCLUDE_DIR} ${CURSES_INCLUDE_DIR} ${FORM_INCLUDE_DIR})
target_link_libraries(snackis c++experimental curl sodium uuid curses form)
