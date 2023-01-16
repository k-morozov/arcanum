cmake_minimum_required(VERSION 3.24)

set(APP_NAME example-generator)

set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -latomic")
find_package(Threads)

add_executable(${APP_NAME} example.cpp)
target_link_libraries(${APP_NAME} ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries(${APP_NAME} generator)

set_target_properties(${APP_NAME} PROPERTIES
        CXX_STANDARD 20
        COMPILE_OPTIONS "-fsanitize=thread"
        LINK_OPTIONS "-fsanitize=thread"
        )