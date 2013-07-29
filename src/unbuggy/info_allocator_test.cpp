#include "unbuggy/info_allocator.hpp"

#include <memory>
#include <vector>

struct item {
};

int main()
{
    // The allocator must be default constructible.

    unbuggy::info_allocator<item, std::allocator<item> > a;

    // The allocator must be copyable.

    auto b = a;
    (void)b;    // Suppress "unused variable" warning.

    // The allocator must be constructible from a different allocator type (to
    // wrap the supplied allocator).

    std::allocator<item> c;
    unbuggy::info_allocator<item, std::allocator<item> > d( c );

    (void)d;    // Suppress "unused variable" warning.

    // The allocator must provide sufficient functionality for use with
    // standard containers.

    std::vector<item, unbuggy::info_allocator<item> > v(a);
}
