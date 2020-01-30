cmake_minimum_required(VERSION 3.14)
project(smp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-O0 -ggdb -std=c++17 -Wall -Werror -Wextra -Wpedantic -Wno-unknown-pragmas")
add_executable(smp tests/extract_modif.cpp)