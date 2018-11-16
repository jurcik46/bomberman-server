cmake_minimum_required(VERSION 3.10)
project(bomberman_server C)

set(CMAKE_C_STANDARD 99)

add_executable(bomberman_server main.c logging/log.c logging/log.h)
