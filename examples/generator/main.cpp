//
// Created by focus on 12.01.23.
//

#include <coroutine>
#include <iostream>

#include "generator.h"
#include "resumable.h"

resumable
foo() {
    std::cout << "Hello" << std::endl;
    co_await std::suspend_always();
    std::cout << "world" << std::endl;
}

generator<int>
natural_nums() {
    std::cout << "[coro] started natural_nums" << std::endl;
    int value = 1;
    while (value < 10) {
        std::cout << "[coro] prepare to coro" << std::endl;
        co_yield value;
        std::cout << "[coro] from coro" << std::endl;
        value++;
    }
}

generator<int>
sequence(int start, int finish, int step) {
    std::cout << "[coro] started natural_nums" << std::endl;
    int num{};
    for (num = start; num < finish; num += step) {
        std::cout << "[coro] prepare to coro" << std::endl;
        co_yield num;
        std::cout << "[coro] from coro" << std::endl;
    }
}

void
example_resumable() {
    auto c = foo();
    std::cout << "size coro = " << sizeof(c) << std::endl;
    c.resume();
    std::cout << "===" << std::endl;
    c.resume();
}

void
example_generator() {
    auto c = natural_nums();
    //	std::cout << "size coro = " << sizeof(c) << std::endl;
    std::cout << "[func] value=" << c.current_value() << std::endl;
    c.move_next();
    std::cout << "[func] value=" << c.current_value() << std::endl;
    c.move_next();
    std::cout << "[func] value=" << c.current_value() << std::endl;
}

void
example_generator_iterator() {
    for (auto value : sequence(2, 32, 2)) {
        std::cerr << "value=" << value << std::endl;
    }
}

int
main() {
    //	example_resumable();
    //	example_generator();
    example_generator_iterator();
}
