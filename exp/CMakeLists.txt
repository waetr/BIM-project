cmake_minimum_required(VERSION 3.17)
project(exp)

set(CMAKE_CXX_STANDARD 14)

MATH(EXPR stack_size "102400000")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--stack,${stack_size}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(exp src/main.cpp include/simulation.h)
