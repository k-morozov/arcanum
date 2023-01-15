//
// Created by focus on 15.01.23.
//

#ifndef ARCANUM_GENERATOR_H
#define ARCANUM_GENERATOR_H

#include <coroutine>
#include <exception>
#include <iterator>

template <typename T>
class generator;

template <typename T>
class generator {
  public:
    struct promise_type;
    using coro_handle = std::coroutine_handle<promise_type>;

    struct promise_type {
        T current_value;
        generator<T> get_return_object() {
            return generator<T>(coro_handle::from_promise(*this));
        }

        auto initial_suspend() {
            std::cout << "[impl] init: " << current_value << std::endl;
            return std::suspend_never();
        }
        auto final_suspend() noexcept {
            return std::suspend_always();
        }

        void return_void() {
        }

        void unhandled_exception() {
            std::terminate();
        }

        // support co_yield
        auto yield_value(T value) {
            std::cout << "[impl] up value to " << value << ", old_value = " << current_value
                      << std::endl;
            current_value = value;
            return std::suspend_always();
        }
    };

    class generator_iterator : public std::iterator<std::input_iterator_tag, T> {
        friend generator<T>;

      public:
        generator_iterator(const generator_iterator& it) : p_(it->p_) {
        }

        bool operator!=(const generator_iterator& other) const {
            //			std::cout << "[iter] op!= " << std::endl;
            return p_ != other.p_;
        }
        T& operator*() const {
            //			std::cout << "[iter] op*" << std::endl;
            return p_->handle_.promise().current_value;
        }
        generator_iterator& operator++() {
            //			std::cout << "[iter] op++" << std::endl;
            p_->handle_.resume();
            if (p_->handle_.done()) {
                p_ = nullptr;
            }
            return *this;
        }

      private:
        generator<T>* p_;
        explicit generator_iterator(generator<T>* p) : p_(p){};
    };

    using iterator = generator_iterator;
    using const_iterator = const generator_iterator;

    explicit generator(coro_handle handle) : handle_(handle) {
    }
    bool operator!=(const generator<T>& rhs) const {
        return this->handle_.promise().current_value != rhs.handle_.promise().current_value;
    }
    ~generator() {
        if (handle_) {
            handle_.destroy();
        }
    }

    T current_value() const {
        return handle_.promise().current_value;
    }
    bool move_next() {
        return handle_ ? (handle_.resume(), !handle_.done()) : false;
    }

    iterator begin() {
        //		std::cout << "[coro] begin" << std::endl;
        if (handle_.done()) {
            return generator_iterator{nullptr};
        }
        return generator_iterator(this);
    }
    iterator end() {
        //		std::cout << "[coro] end" << std::endl;
        return generator_iterator{nullptr};
    }

    const_iterator begin() const {
        if (handle_.done()) {
            return generator_iterator{nullptr};
        }
        return generator_iterator(this);
    }
    const_iterator end() const {
        return generator_iterator{nullptr};
    }

  private:
    coro_handle handle_;
};

#endif // ARCANUM_GENERATOR_H
