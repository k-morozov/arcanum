cmake_minimum_required(VERSION 3.24)
project(arcanum)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_macros.html
    add_compile_options(-D_GLIBCXX_ASSERTIONS -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_LIBCPP_DEBUG=1)
endif()

# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# https://clang.llvm.org/docs/DiagnosticsReference.html
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(arcanum main.cpp)

add_subdirectory(lib/cache)
add_subdirectory(lib/mqueue)
add_subdirectory(lib/generator)

add_subdirectory(examples)