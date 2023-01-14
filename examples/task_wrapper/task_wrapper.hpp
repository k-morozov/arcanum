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
    template <std::size_t N>
    struct StoreManager final {
        template <typename Callable>
        explicit StoreManager(Callable&& c) {
            static_assert(
                sizeof(Callable) <= sizeof(store_.buf()), "small buffer for callable object");
            new (access()) Callable{std::forward<Callable>(c)};
        }

        auto* access() {
            return &callable_store_;
        }
        auto& buf() {
            return callable_store_;
        }

      private:
        std::aligned_storage_t<N> callable_store_{};
    };

  public:
    TaskWrapper() = default;

    template <class Callable>
    explicit TaskWrapper(Callable c)
        : store_{std::move(c)}, vtable_{&vtable_for<Callable, R, Args...>} {
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
    StoreManager<64> store_{};
    const detail::vtable<R, Args...>* vtable_{};
};

#endif // ARCANUM_TASK_WRAPPER_HPP
