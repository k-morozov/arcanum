//
// Created by focus on 11.01.23.
//

#ifndef ARCANUM_TASK_WRAPPER_HPP
#define ARCANUM_TASK_WRAPPER_HPP

#include "vtable_for.h"

template <typename Signature>
class TaskWrapper;

template <typename R, typename... Args>
class TaskWrapper<R(Args...)> final {
    struct StoreManager {
        std::aligned_storage_t<64> buf_{};
        auto* access() {
            return &buf_;
        }
        auto& buf() {
            return buf_;
        }
    };

  public:
    TaskWrapper() = default;

    template <class Callable>
    explicit TaskWrapper(Callable c) : vtable_{&vtable_for<Callable, R, Args...>} {
        static_assert(sizeof(Callable) <= sizeof(store_.buf()), "small buffer for callable object");
        new (store_.access()) Callable{std::move(c)};
    }

    R operator()(Args... args) {
        if (vtable_) {
            return vtable_->run(store_.access(), args...);
        }
        throw std::invalid_argument("broken vtable");
    }

    ~TaskWrapper() {
        if (vtable_) {
            vtable_->destroy(store_.access());
        }
    }

  private:
    StoreManager store_{};
    const detail::vtable<R, Args...>* vtable_{};
};

#endif // ARCANUM_TASK_WRAPPER_HPP
