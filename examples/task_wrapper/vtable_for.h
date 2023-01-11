//
// Created by focus on 11.01.23.
//

#ifndef ARCANUM_VTABLE_FOR_H
#define ARCANUM_VTABLE_FOR_H

#include "vtable.hpp"

#include <memory>

template <class Callable>
constexpr detail::vtable vtable_for{
    [](void* p) { static_cast<Callable*>(p)->operator()(); },
    [](void* p) { std::destroy_at(static_cast<Callable*>(p)); }};

#endif // ARCANUM_VTABLE_FOR_H
