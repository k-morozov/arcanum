//
// Created by focus on 16.01.23.
//

#include <iostream>

#include <core/generator.h>

Generator<int>
simple() {
    int num{};
    while (num < 10) {
        co_yield num;
        num++;
    }
}

Generator<int>
sequence(int start, int finish, int step) {
    int current{};
    for (current = start; current < finish; current += step) {
        co_yield current;
    }
}

int
main() {
    for (int value : sequence(2, 10, 2)) {
        std::cout << value << std::endl;
    }
}