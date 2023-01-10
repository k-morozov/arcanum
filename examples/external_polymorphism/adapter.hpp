//
// Created by focus on 10.01.23.
//

#ifndef ARCANUM_ADAPTER_HPP
#define ARCANUM_ADAPTER_HPP

#include "dump.hpp"
#include "dumpable.hpp"

template <class T>
class DumpableAdapter final : public Dumpable {
  public:
    explicit DumpableAdapter(T* o) : obj_(o) {
    }

  private:
    T* obj_;

    void dump_impl() override {
        detail::dump(*obj_);
    }
};

#endif // ARCANUM_ADAPTER_HPP
