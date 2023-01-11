//
// Created by focus on 11.01.23.
//

#include "task_wrapper.hpp"

#include <iostream>

int
main() {
    TaskWrapper{[] { std::cout << "Hello world!" << std::endl; }}();
    auto f = TaskWrapper{[] { std::cout << "Hello my world!" << std::endl; }};
    f();
}
