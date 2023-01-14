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

template <std::invocable T>
class mqueue_base {
    using mutex_t = std::mutex;
    using lock_guard = std::lock_guard<mutex_t>;
    using unique_lock = std::unique_lock<mutex_t>;
    using pop_t = std::shared_ptr<T>;

    static constexpr int wait_timeout = 1;

  public:
    ~mqueue_base() {
        is_shutdown.store(true);
        cv_.notify_all();
    }

    template <std::invocable U>
    void push(U&& task) {
        if (is_shutdown.load()) {
            return;
        }
        {
            lock_guard lck(m_);
            tasks_.push(std::forward<U>(task));
        }
        cv_.notify_one();
    }

    [[nodiscard]] pop_t wait_and_pop() {
        if (is_shutdown.load()) {
            return {};
        }

        using namespace std::chrono_literals;
        unique_lock lck(m_);
        cv_.wait_for(
            lck, wait_timeout * 1s, [this, &lck] { return !is_empty(lck) || is_shutdown.load(); });
        if (is_shutdown.load()) {
            return {};
        }
        return pop(lck);
    }

    bool empty() const {
        lock_guard lck(m_);
        return tasks_.empty();
    }

  private:
    std::queue<T> tasks_;
    mutable mutex_t m_;
    std::condition_variable cv_;

    std::atomic_bool is_shutdown{false};

    bool is_empty([[maybe_unused]] unique_lock& lck) const {
        return tasks_.empty();
    }

    [[nodiscard]] pop_t pop(unique_lock& lck) {
        if (is_empty(lck)) {
            return {};
        }

        return pop_impl(lck);
    }

    pop_t pop_impl([[maybe_unused]] unique_lock& lck) {
        pop_t res = std::make_shared<T>(std::move(tasks_.front()));
        tasks_.pop();
        return res;
    }
};
