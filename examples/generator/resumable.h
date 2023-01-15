//
// Created by focus on 15.01.23.
//

#ifndef ARCANUM_RESUMABLE_H
#define ARCANUM_RESUMABLE_H

#include <coroutine>
#include <iostream>

struct resumable {
    struct promise_type;
    using coro_handle = std::coroutine_handle<promise_type>;

    struct promise_type {
        resumable get_return_object() {
            return resumable(coro_handle::from_promise(*this));
        }
        auto initial_suspend() {
            std::cout << "init" << std::endl;
            return std::suspend_always();
        }
        auto final_suspend() noexcept {
            std::cout << "final" << std::endl;
            return std::suspend_always();
        }
        void return_void() {
            std::cout << "return" << std::endl;
        }
        void unhandled_exception() {
            std::terminate();
        }
    };

    explicit resumable(coro_handle handle) : handle_{handle} {};
    bool resume() {
        if (!handle_.done()) {
            handle_.resume();
        }
        return !handle_.done();
    }
    ~resumable() {
        handle_.destroy();
    }

  private:
    coro_handle handle_;
};

#endif // ARCANUM_RESUMABLE_H
