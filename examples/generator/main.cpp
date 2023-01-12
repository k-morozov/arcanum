//
// Created by focus on 12.01.23.
//

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
            return std::suspend_always();
        }
        auto final_suspend() noexcept {
            return std::suspend_always();
        }
        void return_void() {
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

resumable
foo() {
    std::cout << "Hello" << std::endl;
    co_await std::suspend_always();
    std::cout << "world" << std::endl;
}

int
main() {
    auto c = foo();
    c.resume();
    std::cout << "===" << std::endl;
    c.resume();
}
