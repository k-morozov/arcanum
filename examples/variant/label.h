//
// Created by focus on 02.02.23.
//

#ifndef ARCANUM_LABEL_H
#define ARCANUM_LABEL_H

#include <concepts>
#include <iostream>

struct Label {
    void print() const {
        std::cout << "call print()" << std::endl;
    }
};

template <typename T>
concept ILabel = requires(const T a) {
                     { a.print() } -> std::convertible_to<void>;
                 };

#endif // ARCANUM_LABEL_H
