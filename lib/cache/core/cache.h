//
// Created by focus on 27.12.22.
//

#pragma once

#include <iterator>
#include <list>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <shared_mutex>

template <typename TKey, typename TValue, template <class> class TPolicy>
class Cache final : public TPolicy<Cache<TKey, TValue, TPolicy>> {
    friend TPolicy<Cache<TKey, TValue, TPolicy>>;

    using mutex_t = std::shared_mutex;

  public:
    explicit Cache(size_t size) : max_table_size(size) { table_.reserve(max_table_size); }
    ~Cache() = default;

    bool put(const TKey &key, const TValue &value);
    std::optional<TValue> get(const TKey &key) const;

  private:
    const size_t max_table_size;
    mutable mutex_t m_;

    using DataIt = typename std::list<std::pair<TKey, TValue>>::iterator;

    std::unordered_map<TKey, DataIt> table_;
    std::list<std::pair<TKey, TValue>> data_;

    bool exists(const TKey &key, std::shared_lock<mutex_t> &lck) const;
    void drop_tail(std::unique_lock<mutex_t> &lck);
};

template <typename TKey, typename TValue, template <class> class TPolicy>
void Cache<TKey, TValue, TPolicy>::drop_tail(std::unique_lock<mutex_t> &lck) {
    if (data_.empty()) {
        return;
    }
    const auto &last = data_.back();
    table_.erase(last.first);
    data_.pop_back();
}

template <typename TKey, typename TValue, template <class> class TPolicy>
bool Cache<TKey, TValue, TPolicy>::exists(const TKey &key, std::shared_lock<mutex_t> &lck) const {
    return table_.contains(key);
}

template <typename TKey, typename TValue, template <class> class TPolicy>
std::optional<TValue> Cache<TKey, TValue, TPolicy>::get(const TKey &key) const {
    std::shared_lock lck(m_);
    if (auto it = table_.find(key); it != table_.end()) {
        return it->second->second;
    }
    return std::nullopt;
}

template <typename TKey, typename TValue, template <class> class TPolicy>
bool Cache<TKey, TValue, TPolicy>::put(const TKey &key, const TValue &value) {
    bool result = false;
    std::shared_lock lck_read(m_);
    if (exists(key, lck_read)) {
        std::unique_lock lck_write(*lck_read.release(), std::adopt_lock);
        if (auto it = table_[key]; it != data_.begin()) {
            data_.splice(data_.begin(), data_, it, std::next(it));
        }
    } else {
        std::unique_lock lck_write(*lck_read.release(), std::adopt_lock);
        data_.push_front(std::make_pair(key, value));
        table_.insert({key, data_.begin()});
        result = true;
        while (data_.size() > max_table_size) {
            drop_tail(lck_write);
        }
    }

    return result;
}