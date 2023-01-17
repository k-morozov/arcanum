//
// Created by focus on 16.01.23.
//

#pragma once

#include <coroutine>
#include <exception>
#include <iterator>

template <typename T>
class Generator final {
    using self_t = Generator<T>;

  public:
    class promise_type;
    using coro_handle_t = std::coroutine_handle<promise_type>;

    struct promise_type {
        T current_value;

        auto get_return_object() {
            return Generator<T>(coro_handle_t::from_promise(*this));
        }
        auto initial_suspend() noexcept {
            return std::suspend_never();
        }
        auto final_suspend() noexcept {
            return std::suspend_always();
        }
        auto unhandled_exception() {
            std::terminate();
        }
        auto yield_value(T new_value) {
            current_value = new_value;
            return std::suspend_always();
        }
    };

    class Iterator : public std::iterator<std::input_iterator_tag, T> {
        friend Generator<T>;

      public:
        Iterator(const Iterator& other) : g_(other.g_) {
        }

        bool operator!=(const Iterator& other) {
            return g_ != other.g_;
        }
        Iterator& operator++() {
            g_->handle_.resume();
            if (g_->handle_.done()) {
                g_ = nullptr;
            }
            return *this;
        }
        const T& operator*() const {
            return g_->handle_.promise().current_value;
        }

      private:
        Generator<T>* g_;

        explicit Iterator(Generator<T>* g) : g_(g) {
        }
    };

    explicit Generator(coro_handle_t handle) : handle_(handle){};
    ~Generator() {
        if (handle_) {
            handle_.destroy();
        }
    }

    T value() const {
        return handle_.promise().current_value;
    }

    bool next() {
        return handle_ && (handle_.resume(), !handle_.done());
    }

    Iterator begin() {
        if (handle_.done()) {
            return Iterator{nullptr};
        }
        return Iterator(this);
    }
    Iterator end() {
        return Iterator(nullptr);
    }

  private:
    coro_handle_t handle_;
};
