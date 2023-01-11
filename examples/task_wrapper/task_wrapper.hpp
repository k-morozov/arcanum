//
// Created by focus on 11.01.23.
//

#ifndef ARCANUM_TASK_WRAPPER_HPP
#define ARCANUM_TASK_WRAPPER_HPP

#include "vtable_for.h"

class TaskWrapper final {
  public:
    TaskWrapper() : vtable_(nullptr) {
    }

    template <class Callable>
    explicit TaskWrapper(Callable c) : vtable_{&vtable_for<Callable>} {
        static_assert(sizeof(Callable) <= sizeof(buf_), "small buffer for callable object");
        new (&buf_) Callable{std::move(c)};
    }

    void operator()() {
        if (vtable_) {
        }
        vtable_->run(&buf_);
    }

    ~TaskWrapper() {
        if (vtable_) {
            vtable_->destroy(&buf_);
        }
    }

  private:
    std::aligned_storage_t<64> buf_{};
    const detail::vtable* vtable_;
};

#endif // ARCANUM_TASK_WRAPPER_HPP
