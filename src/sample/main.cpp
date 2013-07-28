#include "unbuggy/info_allocator.hpp"

#include <iostream>

namespace unb = unbuggy;

struct item {
};

int main()
{
    unb::info_allocator<item> a;

    std::cout << sizeof(a) << '\n';
}
