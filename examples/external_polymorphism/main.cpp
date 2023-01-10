//
// Created by focus on 09.01.23.
//

#include <algorithm>
#include <memory>
#include <ranges>
#include <vector>

#include "adapter.hpp"
#include "some_lib.hpp"

int
main() {
    SomeA a("my_some_a");
    SomeB b(117);

    std::cout << "size A: " << sizeof(a) << std::endl;
    std::cout << "size B: " << sizeof(b) << std::endl;

    std::vector<std::unique_ptr<Dumpable>> data;
    data.push_back(std::make_unique<DumpableAdapter<SomeA>>(&a));
    data.push_back(std::make_unique<DumpableAdapter<SomeB>>(&b));

    std::ranges::for_each(data, std::mem_fn(&Dumpable::dump));
}