//
// Created by focus on 02.02.23.
//

#include <iostream>
#include <variant>

#include "foo.h"

template <typename... Args>
struct overloaded : Args... {
    using Args::operator()...;
};

template <typename... Args>
overloaded(Args...) -> overloaded<Args...>;

std::variant<int, double>
foo(int a) {
    if (a % 2 == 0) {
        return double(1.11);
    } else {
        return int(1);
    }
}

int
main() {
    auto v = foo(1);
    std::visit(
        overloaded{
            [](int a) { std::cout << "int: " << a << std::endl; },
            [](double a) { std::cout << "double: " << a << std::endl; },
        },
        v);

    v = foo(2);
    std::visit(
        overloaded{
            [](int a) { std::cout << "int: " << a << std::endl; },
            [](double a) { std::cout << "double: " << a << std::endl; },
        },
        v);

    const Label label;
    Foo foo;
    foo.run(label);
}
