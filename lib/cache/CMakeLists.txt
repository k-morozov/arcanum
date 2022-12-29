cmake_minimum_required(VERSION 3.24)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
set(CONAN_DISABLE_CHECK_COMPILER True)
conan_basic_setup()

include_directories(.)

add_subdirectory(core)
add_subdirectory(test)