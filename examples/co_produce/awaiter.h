//
// Created by focus on 17.01.23.
//

#ifndef ARCANUM_AWAITER_H
#define ARCANUM_AWAITER_H

#include <coroutine>
#include <list>

using coro_t = std::coroutine_handle<>;

class evt_awaiter_t {
    struct awaiter;

    awaiter* top_;
    bool set_;

    struct awaiter {
        explicit awaiter(evt_awaiter_t& event) noexcept : event_(event), next_{} {};

        evt_awaiter_t& event_;
        awaiter* next_;
        coro_t coro_ = nullptr;

        [[nodiscard]] bool await_ready() const noexcept {
            return event_.is_set();
        }
        void await_resume() {
            std::cout << "[awaiter] resume" << std::endl;
            event_.reset();
        }
        void await_suspend(coro_t coro) noexcept {
            coro_ = coro;
            event_.push_awaiter(this);
            std::cout << "[awaiter] suspend" << std::endl;
        }
    };

  public:
    explicit evt_awaiter_t(bool is_set = false) : top_{}, set_(is_set) {
    }

    awaiter operator co_await() noexcept {
        return awaiter{*this};
    }

  public:
    [[nodiscard]] bool is_set() const noexcept {
        return set_;
    }
    void push_awaiter(awaiter* a) {
        a->next_ = top_;
        top_ = a;
    }
    void set() noexcept {
        std::cout << "[awaiter] call set" << std::endl;
        set_ = true;
        awaiter* cur = top_;
        top_ = nullptr;
        while (cur) {
            cur->coro_.resume();
            cur = cur->next_;
        }
    }
    void reset() {
        set_ = false;
    }
};

#endif // ARCANUM_AWAITER_H
