//
// Created by focus on 07.01.23.
//

#pragma once

#include <chrono>
#include <concepts>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <stop_token>

template <std::invocable T>
class mqueue_base {
    using mutex_t = std::mutex;
    using lock_guard = std::lock_guard<mutex_t>;
    using unique_lock = std::unique_lock<mutex_t>;
    using pop_t = std::shared_ptr<T>;

    static constexpr int wait_timeout = 1;

  public:
    ~mqueue_base() {
        stop_.request_stop();
        // @TODO notify and wait?
    }

    template <std::invocable U>
    void push(U&& task) {
        auto token = stop_.get_token();
        if (token.stop_requested()) {
            return;
        }

        {
            lock_guard lck(m_);
            tasks_.push(std::forward<U>(task));
        }
        cv_.notify_one();
    }

    [[nodiscard]] pop_t wait_and_pop() {
        auto token = stop_.get_token();
        if (token.stop_requested()) {
            return {};
        }

        using namespace std::chrono_literals;

        unique_lock lck(m_);
        while (true) {
            const bool is_timeout = !cv_.wait_for(lck, wait_timeout * 1s, [this, &lck, &token] {
                return !tasks_.empty() || token.stop_requested();
            });
            if (token.stop_requested()) {
                return {};
            }
            if (!is_empty(lck)) {
                break;
            }
            if (is_timeout) {
                return {};
            }
        };

        return pop_impl(lck);
    }

    bool empty() const {
        lock_guard lck(m_);
        return tasks_.empty();
    }

  private:
    std::queue<T> tasks_;
    mutable mutex_t m_;
    std::condition_variable cv_;
    std::stop_source stop_;

    bool is_empty([[maybe_unused]] unique_lock& lck) const {
        return tasks_.empty();
    }

    [[nodiscard]] pop_t pop_impl([[maybe_unused]] unique_lock& lck) {
        pop_t res = std::make_shared<T>(std::move(tasks_.front()));
        tasks_.pop();
        return res;
    }
};
