//
// Created by focus on 11.01.23.
//

#ifndef ARCANUM_VTABLE_FOR_H
#define ARCANUM_VTABLE_FOR_H

#include "vtable.hpp"

#include <memory>

template <class Callable, typename R, typename... Args>
constexpr detail::vtable<R, Args...> vtable_for{
    [](void* p, Args... args) -> R {
        return static_cast<Callable*>(p)->operator()(std::forward<Args>(args)...);
    },
    [](void* p) { std::destroy_at(static_cast<Callable*>(p)); }};

#endif // ARCANUM_VTABLE_FOR_H
