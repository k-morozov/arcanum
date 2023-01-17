//
// Created by focus on 17.01.23.
//

#ifndef ARCANUM_RESUMABLE_NO_OWN_H
#define ARCANUM_RESUMABLE_NO_OWN_H

#include <coroutine>
#include <exception>

struct resumable_no_own {
    struct promise_type {
        auto get_return_object() {
            return resumable_no_own{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        auto initial_suspend() {
            return std::suspend_never();
        }
        auto final_suspend() noexcept {
            return std::suspend_never();
        }
        void unhandled_exception() {
            std::terminate();
        }
    };

    resumable_no_own(std::coroutine_handle<promise_type>) {
    }
    resumable_no_own(const resumable_no_own&) {
    }
    resumable_no_own(resumable_no_own&&) {
    }
};

#endif // ARCANUM_RESUMABLE_NO_OWN_H
