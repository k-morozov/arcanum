//
// Created by focus on 10.01.23.
//

#ifndef ARCANUM_SOME_LIB_HPP
#define ARCANUM_SOME_LIB_HPP

#include <string>
#include <string_view>

class SomeA final {
  public:
    explicit SomeA(std::string name) : name_(std::move(name)) {
    }
    [[nodiscard]] std::string_view get_name() const {
        return name_;
    }

  private:
    const std::string name_;
};

class SomeB final {
  public:
    explicit SomeB(const int code) : code_(code) {
    }
    [[nodiscard]] int get_code() const {
        return code_;
    }

  private:
    const int code_;
};

#endif // ARCANUM_SOME_LIB_HPP
