//
// Created by focus on 30.12.22.
//

#pragma once

#include <concepts>
#include <functional>

template <typename T>
concept Hashable = requires(T a) {
                       { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
                   };

// template<typename T, typename U>
// concept Equal = requires(T a, U b) {
//
// };