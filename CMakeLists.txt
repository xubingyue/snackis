cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_COMPILER clang++)
project(snackis)

add_compile_options(-std=c++1z -stdlib=libc++ -Wall -Werror -O2)
set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")

file(GLOB_RECURSE core_src src/snackis/core/*.cpp)
file(GLOB_RECURSE crypt_src src/snackis/crypt/*.cpp)
file(GLOB_RECURSE db_src src/snackis/db/*.cpp)
file(GLOB_RECURSE net_src src/snackis/net/*.cpp)
file(GLOB snackis_src src/snackis/*.cpp)

add_library(libsnackis STATIC ${core_src} ${crypt_src} ${db_src} ${net_src} ${snackis_src})
target_include_directories(libsnackis PUBLIC src/)
target_link_libraries(libsnackis libc++abi.a libc++experimental.a libsodium.a libuuid.a curl pthread)
set_target_properties(libsnackis PROPERTIES PREFIX "")

add_executable(tests EXCLUDE_FROM_ALL ${core_src} ${crypt_src} ${db_src} ${net_src} ${snackis_src} src/tests.cpp)
target_include_directories(tests PUBLIC src/)
target_link_libraries(tests libc++abi.a libc++experimental.a libsodium.a libuuid.a curl pthread)

file(GLOB_RECURSE ui_src src/snackis/ui/*.cpp)

add_executable(snackis ${core_src} ${crypt_src} ${db_src} ${net_src} ${snackis_src} ${ui_src} src/main.cpp)
target_include_directories(snackis PUBLIC src/)
target_link_libraries(snackis libc++abi.a libc++experimental.a libsodium.a libuuid.a curl pthread curses form)

file(GLOB core_inc src/snackis/core/*.hpp)
install(FILES ${core_inc} DESTINATION include/snackis/core)

file(GLOB crypt_inc src/snackis/crypt/*.hpp)
install(FILES ${crypt_inc} DESTINATION include/snackis/crypt)

file(GLOB db_inc src/snackis/db/*.hpp)
install(FILES ${db_inc} DESTINATION include/snackis/db)

file(GLOB net_inc src/snackis/net/*.hpp)
install(FILES ${net_inc} DESTINATION include/snackis/net)

file(GLOB snackis_inc src/snackis/*.hpp)
install(FILES ${snackis_inc} DESTINATION include/snackis)

install(FILES build/libsnackis.a DESTINATION lib)

