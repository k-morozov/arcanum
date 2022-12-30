//
// Created by focus on 29.12.22.
//

#pragma once

#include <iterator>
#include <list>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <unordered_map>

#include "utils/requires.h"

template <Hashable TKey, typename TValue>
class LruPolicy {
    using mutex_t = std::shared_mutex;

  public:
    template <typename T, typename U>
        requires(
            std::same_as<std::decay_t<TKey>, std::decay_t<T>> &&
            std::same_as<std::decay_t<TValue>, std::decay_t<U>>) bool
    put_impl(T&& key, U&& value);

    template <typename T>
        requires std::same_as<std::decay_t<TKey>, std::decay_t<T>>
    std::optional<TValue> get_impl(T&& key) const;

    void set_max_cache_size_impl(size_t new_size);

  protected:
    size_t max_table_size{10};
    mutable mutex_t m_;

    using DataIt = typename std::list<std::pair<TKey, TValue>>::iterator;

    std::unordered_map<TKey, DataIt> table_;
    std::list<std::pair<TKey, TValue>> data_;

    bool exists(const TKey& key, std::shared_lock<mutex_t>& lck) const;
    void drop_tail(std::unique_lock<mutex_t>& lck);
};

template <Hashable TKey, typename TValue>
void
LruPolicy<TKey, TValue>::set_max_cache_size_impl(const size_t new_size) {
    std::unique_lock lck(m_);
    max_table_size = new_size;
    table_.reserve(max_table_size);
}

template <Hashable TKey, typename TValue>
void
LruPolicy<TKey, TValue>::drop_tail([[maybe_unused]] std::unique_lock<mutex_t>& lck) {
    if (data_.empty()) {
        return;
    }
    const auto& last = data_.back();
    table_.erase(last.first);
    data_.pop_back();
}

template <Hashable TKey, typename TValue>
bool
LruPolicy<TKey, TValue>::exists(
    const TKey& key, [[maybe_unused]] std::shared_lock<mutex_t>& lck) const {
    return table_.contains(key);
}

template <Hashable TKey, typename TValue>
template <typename T>
    requires std::same_as<std::decay_t<TKey>, std::decay_t<T>>
std::optional<TValue>
LruPolicy<TKey, TValue>::get_impl(T&& key) const {
    std::shared_lock lck(m_);
    if (auto it = table_.find(std::forward<T>(key)); it != table_.end()) {
        return it->second->second;
    }
    return std::nullopt;
}

template <Hashable TKey, typename TValue>
template <typename T, typename U>
    requires(
        std::same_as<std::decay_t<TKey>, std::decay_t<T>> &&
        std::same_as<std::decay_t<TValue>, std::decay_t<U>>) bool
LruPolicy<TKey, TValue>::put_impl(T&& key, U&& value) {
    bool result = false;
    std::shared_lock lck_read(m_);
    if (exists(key, lck_read)) {
        std::unique_lock lck_write(*lck_read.release(), std::adopt_lock);
        if (auto it = table_[std::forward<T>(key)]; it != data_.begin()) {
            data_.splice(data_.begin(), data_, it, std::next(it));
        }
    } else {
        std::unique_lock lck_write(*lck_read.release(), std::adopt_lock);
        data_.push_front(std::make_pair(key, std::forward<U>(value)));
        table_.insert({std::forward<T>(key), data_.begin()});
        result = true;
        while (data_.size() > max_table_size) {
            drop_tail(lck_write);
        }
    }

    return result;
}