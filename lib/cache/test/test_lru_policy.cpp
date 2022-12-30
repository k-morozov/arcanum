//
// Created by focus on 27.12.22.
//
#include "gtest/gtest.h"

#include <algorithm>
#include <string>

#include <core/cache.h>
#include <policy/lru_policy.h>
#include <policy/test_policy.h>

class Basic : public ::testing::Test {};

TEST_F(Basic, add_one_element) {
    constexpr int key = 1;
    const std::string value = "value";

    Cache<int, std::string, LruPolicy, TestPolicy> cache(10);
    cache.put(key, value);

    const auto& table = cache.get_table();
    ASSERT_EQ(table.size(), 1);
    ASSERT_TRUE(table.contains(1));

    const auto& data = cache.get_data();
    ASSERT_EQ(data.size(), 1);
    const auto data_it = data.begin();
    ASSERT_EQ(data_it->first, key);
    ASSERT_EQ(data_it->second, value);

    const auto table_it = table.at(key);
    ASSERT_EQ(table_it, data_it);

    const auto elem = cache.get(key);
}

TEST_F(Basic, get_element) {
    constexpr int key = 1;
    const std::string value = "value";

    Cache<int, std::string, LruPolicy, TestPolicy> cache(10);

    auto result_value = cache.get(key);
    ASSERT_EQ(result_value, std::nullopt);

    cache.put(key, value);

    result_value = cache.get(key);
    ASSERT_EQ(result_value, value);

    constexpr int no_key = 2;
    result_value = cache.get(no_key);
    ASSERT_EQ(result_value, std::nullopt);
}

TEST_F(Basic, put_some_elements) {
    constexpr int size = 100;
    std::vector<std::pair<int, std::string>> expected_values(size);

    Cache<int, std::string, LruPolicy, TestPolicy> cache(size);
    for (int i = 0; i < size; ++i) {
        expected_values.emplace_back(i, std::to_string(i));
        cache.put(i, std::to_string(i));
    }

    std::reverse(expected_values.begin(), expected_values.end());

    const auto& data = cache.get_data();
    ASSERT_EQ(data.size(), size);

    // @TODO generator
    for (auto [it, result] = std::tuple{data.cbegin(), expected_values.cbegin()}; it != data.cend();
         it++, result++) {
        ASSERT_EQ(result->first, it->first);
        ASSERT_EQ(result->second, it->second);
    }
}

TEST_F(Basic, put_1000_equal_element) {
    constexpr int key = 1;
    const std::string value = "one";
    constexpr size_t size = 1;

    Cache<int, std::string, LruPolicy, TestPolicy> cache(size);
    bool expected_put = true;

    for (int i = 0; i < 1000; i++) {
        const bool status = cache.put(key, value);
        ASSERT_EQ(status, expected_put);
        expected_put = false;

        const auto result_value = cache.get(key);
        ASSERT_EQ(*result_value, value);

        const auto& table = cache.get_table();
        ASSERT_EQ(table.size(), size);

        const auto& data = cache.get_data();
        ASSERT_EQ(data.size(), size);
        ASSERT_EQ(*data.begin(), std::make_pair(key, value));
    }
}

TEST_F(Basic, put_1000_equal_element_and_one_another) {
    constexpr int key = 2;
    const std::string value = "two";
    constexpr size_t size = 2;

    Cache<int, std::string, LruPolicy, TestPolicy> cache(size);

    bool expected_put = true;
    bool status = cache.put(1, std::string("one"));
    ASSERT_EQ(status, expected_put);

    const auto& data = cache.get_data();
    ASSERT_EQ(data.size(), 1);
    ASSERT_EQ(*data.begin(), std::make_pair(1, std::string("one")));

    for (int i = 0; i < 1000; i++) {
        status = cache.put(key, value);
        ASSERT_EQ(status, expected_put);
        expected_put = false;

        const auto result_value = cache.get(key);
        ASSERT_EQ(*result_value, value);

        const auto& table = cache.get_table();
        ASSERT_EQ(table.size(), size);

        ASSERT_EQ(data.size(), size);
        auto result = data.begin();
        ASSERT_EQ(*result, std::make_pair(key, value));
        ASSERT_EQ(*std::next(result), std::make_pair(1, std::string("one")));
    }
}

TEST_F(Basic, put_1000_diffrent_elements_limit_10) {
    constexpr size_t max_size = 10;

    Cache<int, std::string, LruPolicy, TestPolicy> cache(max_size);

    for (int i = 0; i < max_size; i++) {
        const bool status = cache.put(i, std::to_string(i));
        ASSERT_TRUE(status);
    }

    for (int i = max_size; i < 10 * max_size; i++) {
        const bool status = cache.put(i, std::to_string(i));
        ASSERT_TRUE(status);

        const auto& data = cache.get_data();
        const auto& table = cache.get_table();

        ASSERT_EQ(data.size(), max_size);
        ASSERT_EQ(table.size(), max_size);

        int current = i;
        for (const auto& [k, v] : data) {
            ASSERT_EQ(k, current);
            ASSERT_EQ(v, std::to_string(current));

            ASSERT_TRUE(table.contains(k));
            ASSERT_EQ(*table.at(k), std::make_pair(current, std::to_string(current)));

            --current;
        }
    }
}

TEST_F(Basic, wo_test_policy) {
    constexpr int key = 1;
    const std::string value = "value";

    Cache<int, std::string, LruPolicy> cache(10);
    cache.put(key, value);
    const auto result = cache.get(key);
    ASSERT_TRUE(result != std::nullopt);
    ASSERT_EQ(result.value(), value);
}

TEST_F(Basic, add_one_rv_element) {
    std::string key = "11111";
    const std::string value = "value";

    Cache<std::string, std::string, LruPolicy, TestPolicy> cache(10);
    cache.put(key, value);

    const auto result = cache.get(std::move(key));
    ASSERT_TRUE(result != std::nullopt);
    ASSERT_EQ(result.value(), value);
}