cmake_minimum_required(VERSION 3.10)
project(bomberman_server C)

set(CMAKE_C_STANDARD 99)

add_executable(bomberman_server main.c logging/log.c logging/log.h database.c database.h constants.h cmake-build-debug/ads.cpp)
include_directories(/usr/include/)
link_directories(/usr/lib)
target_link_libraries(bomberman_server sqlite3)