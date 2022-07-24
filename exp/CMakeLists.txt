 cmake_minimum_required(VERSION 3.10)
project(exp)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-O3")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#add_executable(exp src/main.cpp)
add_executable(exp src/test.cpp)
