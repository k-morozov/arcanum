//
// Created by focus on 10.01.23.
//

#ifndef ARCANUM_DUMPABLE_HPP
#define ARCANUM_DUMPABLE_HPP

class Dumpable {
  public:
    void dump() {
        dump_impl();
    }
    virtual ~Dumpable() = default;

  private:
    virtual void dump_impl() = 0;
};

#endif // ARCANUM_DUMPABLE_HPP
