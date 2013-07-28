#include "unbuggy/info_allocator.hpp"

#include <memory>
#include <vector>

struct item {
};

int main()
{
    unbuggy::info_allocator<item, std::allocator<item> > a;
    (void)a;

    std::vector<item, unbuggy::info_allocator<item> > v(a);
}
