cmake_minimum_required(VERSION 3.14)
project(libstevia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O0 -ggdb -std=c++17 -Wall -Werror -Wextra -Wpedantic -Wno-unknown-pragmas")

add_library(${CMAKE_PROJECT_NAME} STATIC
        include/stevia.h)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(contrib/googletest)
add_subdirectory(tests)