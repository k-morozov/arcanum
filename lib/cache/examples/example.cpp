//
// Created by focus on 01.01.23.
//

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

struct game {
    virtual ~game() = default;
    virtual void run() = 0;
};

struct game_base : game {
  public:
    void run() final {
        start_game();

        std::thread ping = std::thread(&game_base::ping, this);
        std::thread pong = std::thread(&game_base::pong, this);

        if (ping.joinable()) {
            ping.join();
        }
        if (pong.joinable()) {
            pong.join();
        }
    }

  protected:
    static constexpr int MaxCountTimes = 100'000;

    virtual void start_game() = 0;
    virtual void ping() = 0;
    virtual void pong() = 0;
};

class game_cv final : public game_base {
    void ping() override {
        int counter = 0;

        while (counter <= MaxCountTimes) {
            {
                std::unique_lock lck(m_);
                cv_.wait(lck, [this]() { return ping_done_; });
                ping_done_ = false;
                pong_done_ = true;
                counter++;
            }
            cv_.notify_one();
        }
    }

    void pong() override {
        int counter = 0;

        while (counter < MaxCountTimes) {
            {
                std::unique_lock lck(m_);
                cv_.wait(lck, [this]() { return pong_done_; });
                ping_done_ = true;
                pong_done_ = false;
                counter++;
            }
            cv_.notify_one();
        }
    }

    void start_game() override {
        {
            std::unique_lock lck(m_);
            ping_done_ = true;
        }
        cv_.notify_one();
    }

  private:
    std::condition_variable cv_;
    std::mutex m_;
    bool ping_done_{false};
    bool pong_done_{false};
};

class game_atomic final : public game_base {
    void ping() override {
        int counter = 0;

        while (counter <= MaxCountTimes) {
            pass_.wait(false);
            pass_.clear();
            counter++;
            current_counter_++;
            pass_.notify_one();
        }
    }

    void pong() override {
        int counter = 0;

        while (counter < MaxCountTimes) {
            pass_.wait(true);

            pass_.test_and_set();
            counter++;
            pass_.notify_one();
        }
    }

    void start_game() override {
        pass_.test_and_set();
        pass_.notify_one();
    }

  private:
    std::atomic_flag pass_;
    std::atomic_int current_counter_;
};

class time_game : public game {
  public:
    explicit time_game(std::unique_ptr<game> p) : game_(std::move(p)) {
    }
    void run() override {
        auto start = std::chrono::system_clock::now();

        game_->run();

        std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
        std::cout << "Duration: " << dur.count() << " seconds" << std::endl;
    }

  private:
    std::unique_ptr<game> game_;
};

int
main() {
    time_game{std::make_unique<game_cv>()}.run();
    time_game{std::make_unique<game_atomic>()}.run();
}