//
// Created by focus on 11.01.23.
//

#ifndef ARCANUM_VTABLE_HPP
#define ARCANUM_VTABLE_HPP

namespace detail {
struct vtable {
    void (*run)(void* p);
    void (*destroy)(void* p);
};
} // namespace detail

#endif // ARCANUM_VTABLE_HPP
