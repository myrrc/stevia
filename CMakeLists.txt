cmake_minimum_required(VERSION 3.14)
project(smp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-O0 -ggdb")
add_executable(smp main.cpp)