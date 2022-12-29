cmake_minimum_required(VERSION 3.24)
project(arcanum)

set(CMAKE_CXX_STANDARD 20)

add_executable(arcanum main.cpp)

add_subdirectory(lib/cache)
