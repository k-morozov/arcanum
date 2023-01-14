//
// Created by focus on 07.01.23.
//
#include "gtest/gtest.h"

#include <future>
#include <unordered_map>
#include <vector>

#include <core/mqueue_base/mqueue_base.h>

class test_queue : public mqueue_base<std::function<void()>> {};

struct Config final {
    const int count_producers = 1;
    const int count_consumers = 1;
    const int count_task_for_producer = 100;
    const int task_size = count_producers * count_task_for_producer;
};

class Basic : public ::testing::Test {
  public:
    void
    run(const int count_producers, const int count_consumers, const int count_task_for_producer) {
        const Config config{
            .count_producers = count_producers,
            .count_consumers = count_consumers,
            .count_task_for_producer = count_task_for_producer,
        };

        test_queue q;

        schedule_tasks(q, config);
        wait_produce(config.count_producers);

        execute_tasks(q, config.count_consumers);
        wait_consume(q, config);

        ASSERT_TRUE(q.empty()) << "count_producers=" << count_producers
                               << "count_consumers=" << count_consumers
                               << "count_task_for_producer=" << count_task_for_producer;

        check_data(config.task_size);

        fproducers_.clear();
        fconsumers_.clear();
        data_.clear();
    }

    void schedule_tasks(test_queue& q, const Config config) {
        for (int i = 0; i < config.count_producers; i++) {
            fproducers_.emplace_back(
                std::make_shared<future_t>(std::async(std::launch::async, [&q, i, config, this] {
                    int push_tasks = 0;
                    for (int j = config.count_task_for_producer * i;
                         j < config.count_task_for_producer * (i) + config.count_task_for_producer;
                         j++) {
                        q.push([j, this] {
                            std::lock_guard lck(m);
                            data_[j]++;
                        });
                        push_tasks++;
                    }
                    return push_tasks;
                })));
        };
        fconsumers_.reserve(config.count_consumers);
    }

    void wait_produce(const int count_worker) {
        ASSERT_EQ(fproducers_.size(), count_worker);
        int all_tasks = 0;
        for (auto& f : fproducers_) {
            ASSERT_TRUE(f) << "count_worker=" << count_worker;
            const int current_task = (*f).get();
            std::cerr << "producer push tasks=" << current_task << std::endl;
            all_tasks += current_task;
        }
        std::cerr << "produce tasks=" << all_tasks << std::endl;
    }

  private:
    using future_t = std::future<int>;
    using task_t = std::shared_ptr<future_t>;

    using future_consume_t = std::future<int>;
    using task_consume_t = std::shared_ptr<future_consume_t>;

    std::mutex m;
    std::unordered_map<int, int> data_;

    std::vector<task_t> fproducers_;
    std::vector<task_consume_t> fconsumers_;

    void execute_tasks(test_queue& q, const int count_consumers) {
        for (int i = 0; i < count_consumers; i++) {
            fconsumers_.emplace_back(
                std::make_shared<future_consume_t>(std::async(std::launch::async, [&q]() -> int {
                    int done_task = 0;
                    while (!q.empty()) {
                        auto task = q.wait_and_pop();
                        if (!task) {
                            std::cerr << "broken task" << std::endl;
                            continue;
                        }
                        (*task)();
                        done_task++;
                    }
                    return done_task;
                })));
        }
    }

    void wait_consume(const test_queue& test_queue, const Config config) {
        std::cerr << "===[p=" << config.count_producers << "]====[c=" << config.count_consumers
                  << "]===[tasks=" << config.count_producers * config.count_task_for_producer
                  << "]===" << std::endl;
        ASSERT_EQ(fconsumers_.size(), config.count_consumers);
        int all_done_task = 0;
        for (auto& f : fconsumers_) {
            ASSERT_TRUE(f);
            const int current_task = (*f).get();
            std::cerr << "consume done task=" << current_task << std::endl;
            all_done_task += current_task;
        }
        std::cerr << "======="
                  << "[q is_empty=" << std::boolalpha << test_queue.empty()
                  << "]========" << std::endl
                  << std::endl;
        ASSERT_TRUE(test_queue.empty());
        ASSERT_EQ(all_done_task, config.count_producers * config.count_task_for_producer);
    }

    void check_data(const int size) const {
        ASSERT_EQ(data_.size(), size);
        for (int i = 0; i < size; i++) {
            ASSERT_EQ(data_.at(i), 1);
        }
    }
};

TEST_F(Basic, test_small_count_task_many_workers) {
    constexpr int count_task_for_producer = 100;
    for (int producers = 1; producers <= 32; producers *= 2) {
        for (int consumers = 1; consumers <= 32; consumers *= 2) {
            run(producers, consumers, count_task_for_producer);
        }
    }
}

TEST_F(Basic, test_big_count_task_few_workers) {
    constexpr int count_task_for_producer = 10'000;
    for (int producers = 1; producers <= 8; producers *= 2) {
        for (int consumers = 1; consumers <= 8; consumers *= 2) {
            run(producers, consumers, count_task_for_producer);
        }
    }
}

TEST_F(Basic, test_big_count_task_two_workers) {
    constexpr int count_task_for_producer = 1'000'000;
    run(2, 2, count_task_for_producer);
}

TEST_F(Basic, test_no_task) {
    constexpr int count_task_for_producer = 0;
    run(2, 2, count_task_for_producer);
}

TEST_F(Basic, test_early_shutdown_after_schedule) {
    const Config config{
        .count_producers = 8,
        .count_consumers = 8,
        .count_task_for_producer = 1'000,
    };
    {
        test_queue q;

        schedule_tasks(q, config);
        wait_produce(config.count_producers);
    }
}