//
// Created by focus on 11.01.23.
//

#ifndef ARCANUM_VTABLE_HPP
#define ARCANUM_VTABLE_HPP

namespace detail {

template <typename R, typename... Args>
struct vtable {
    R (*run)(void* p, Args... args);
    void (*destroy)(void* p);
};
} // namespace detail

#endif // ARCANUM_VTABLE_HPP
