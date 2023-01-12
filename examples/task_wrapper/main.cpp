//
// Created by focus on 11.01.23.
//

#include "task_wrapper.hpp"

#include <functional>
#include <iostream>

int
main() {
    TaskWrapper{[] { std::cout << "Hello world!" << std::endl; }}();
    auto w = TaskWrapper{[] { std::cout << "Hello my world!" << std::endl; }};
    w();
    std::cout << sizeof(w) << std::endl;

    std::function f = [] { std::cout << "Hello my world!" << std::endl; };
    std::cout << sizeof(f) << std::endl;
}
