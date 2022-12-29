cmake_minimum_required(VERSION 3.24)
project(cache)

set(CMAKE_CXX_STANDARD 20)

add_library(cache cache.cpp)
target_link_libraries(cache ${CONAN_LIBS})