//
// Created by focus on 27.12.22.
//

#pragma once

#include <iterator>
#include <list>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <unordered_map>

#include "utils/requires.h"

template <class Derived>
class TestPolicy;

template <
    Hashable TKey, typename TValue, template <class...> class TCachePolicy,
    template <class> class... TPolicy>
class Cache final : private TCachePolicy<TKey, TValue>,
                    public TPolicy<Cache<TKey, TValue, TCachePolicy, TPolicy...>>... {
    using self_t = Cache<TKey, TValue, TCachePolicy, TPolicy...>;
    friend class TestPolicy<self_t>;

    using TCachePolicy<TKey, TValue>::get_impl;
    using TCachePolicy<TKey, TValue>::put_impl;
    using TCachePolicy<TKey, TValue>::set_max_cache_size_impl;

  public:
    explicit Cache(const size_t size) { set_max_cache_size(size); }
    ~Cache() = default;

    template <typename T>
    inline std::optional<TValue> get (T&& key) const {
        return get_impl(std::forward<T>(key));
    }

    template <typename T, typename U>
    inline bool put (T&& key, U&& value) {
        return put_impl(std::forward<T>(key), std::forward<U>(value));
    }

    inline void set_max_cache_size (const size_t size) { set_max_cache_size_impl(size); }
};