//
// Created by focus on 11.01.23.
//

#include "task_wrapper.hpp"

#include <functional>
#include <iostream>

int
main() {
    TaskWrapper<void()>{[] { std::cout << "Hello world!" << std::endl; }}();
    auto w = TaskWrapper<void()>{[] { std::cout << "Hello my world!" << std::endl; }};
    w();
    std::cout << sizeof(w) << std::endl;

    std::function f = [] { std::cout << "Hello my world!" << std::endl; };
    std::cout << sizeof(f) << std::endl;

    auto wr = TaskWrapper<int()>([] { return 112; });
    std::cout << wr() << std::endl;

    auto wrp = TaskWrapper<int(int)>([](int a) { return a; });
    std::cout << wrp(113) << std::endl;
}
