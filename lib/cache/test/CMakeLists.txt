cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)

set(APP_NAME test_cache)

add_executable(${APP_NAME})

set_target_properties(${APP_NAME} PROPERTIES
        CXX_STANDARD 20
        COMPILE_OPTIONS "-fsanitize=thread"
        LINK_OPTIONS "-fsanitize=thread"
        )

target_sources(${APP_NAME} PUBLIC
        main.cpp
        basic.cpp
        )

include_directories(${CONAN_INCLUDE_DIRS})
target_link_libraries(${APP_NAME}
        ${CONAN_LIBS}
        cache
        )

add_test(NAME ${APP_NAME} COMMAND $<TARGET_FILE:${APP_NAME}>)