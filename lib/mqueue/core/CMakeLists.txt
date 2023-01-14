cmake_minimum_required(VERSION 3.24)
set(LIB_NAME mqueue)
project(${LIB_NAME})

set(CMAKE_CXX_STANDARD 20)

add_library(${LIB_NAME} mqueue_base/mqueue_base.cpp)
target_link_libraries(${LIB_NAME} ${CONAN_LIBS})