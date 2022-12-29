//
// Created by focus on 28.12.22.
//

#pragma once

template <class Derived> class TestPolicy {
  public:
    const auto &get_table() const { return derived()->table_; }

    const auto &get_data() const { return derived()->data_; }

  private:
    const Derived *derived() const { return static_cast<const Derived *>(this); }
    Derived *derived() { return static_cast<Derived *>(this); }
};