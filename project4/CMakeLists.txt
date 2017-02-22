cmake_minimum_required(VERSION 3.5)
project(project4)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c params.h)
add_executable(project4 ${SOURCE_FILES})