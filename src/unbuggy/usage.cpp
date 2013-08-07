#include "unbuggy/fml.hpp"

#include <iostream>
#include <scoped_allocator>
#include <string>
#include <vector>

// Suggest:
// - remove _now suffixes
// - change allocated_memory to memory

int main()
{
    // How much memory is my simple container using?

    std::vector<int, unbuggy::counting_allocator<int> > v;

    for (int i = 0; i < 1000; ++i)
        v.push_back(i);

    std::cout << v.get_allocator().memory() << '\n';
}
