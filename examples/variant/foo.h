//
// Created by focus on 02.02.23.
//

#ifndef ARCANUM_FOO_H
#define ARCANUM_FOO_H

#include "label.h"

struct Foo {
    void run(ILabel auto lable) {
        lable.print();
    }
};

#endif // ARCANUM_FOO_H
