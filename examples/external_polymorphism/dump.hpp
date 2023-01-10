//
// Created by focus on 10.01.23.
//

#ifndef ARCANUM_DUMP_HPP
#define ARCANUM_DUMP_HPP

#include <iostream>

#include "some_lib.hpp"

namespace detail {

template <class T>
void
dump(const T&) {
    std::cout << "undefined output" << std::endl;
}

template <>
void
dump<SomeA>(const SomeA& a) {
    std::cout << "SomeA: " << a.get_name() << std::endl;
}

template <>
void
dump<SomeB>(const SomeB& b) {
    std::cout << "SomeB: " << b.get_code() << std::endl;
}

} // namespace detail

#endif // ARCANUM_DUMP_HPP
