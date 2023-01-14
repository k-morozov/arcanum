cmake_minimum_required(VERSION 3.24)
set(LIB_NAME mqueue)
project(${LIB_NAME})

add_library(${LIB_NAME} mqueue_base/mqueue_base.cpp)
target_link_libraries(${LIB_NAME} ${CONAN_LIBS})

set_target_properties(${LIB_NAME} PROPERTIES
        CXX_STANDARD 20
        COMPILE_OPTIONS "-fsanitize=thread"
        LINK_OPTIONS "-fsanitize=thread"
        )