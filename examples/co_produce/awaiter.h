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

    std::list<awaiter> lst_;
    bool set_;

    struct awaiter {
        explicit awaiter(evt_awaiter_t& event) noexcept : event_(event){};

        evt_awaiter_t& event_;
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
            event_.push_awaiter(*this);
            std::cout << "[awaiter] suspend" << std::endl;
        }
    };

  public:
    explicit evt_awaiter_t(bool is_set = false) : set_(is_set) {
    }
    awaiter operator co_await() noexcept {
        return awaiter{*this};
    }

  public:
    bool is_set() const noexcept {
        return set_;
    }
    void push_awaiter(awaiter a) {
        lst_.push_back(a);
    }
    void set() noexcept {
        std::cout << "[awaiter] call set" << std::endl;
        set_ = true;
        std::list<awaiter> to_resume;
        to_resume.splice(to_resume.begin(), lst_);
        for (auto s : to_resume) {
            s.coro_.resume();
        }
    }
    void reset() {
        set_ = false;
    }
};

#endif // ARCANUM_AWAITER_H
