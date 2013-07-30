/** @file info_allocator_test.cpp */

#include "unbuggy/info_allocator.hpp"

#include <memory>
#include <vector>

/// Dummy type for use as a parameter to allocator class templates.
struct item { };

int main()
{
    namespace unb = unbuggy;

    // TODO Check [allocator.requirements].

    // Check expectations not required by the standard.

    unb::info_allocator<item> a;                    // default constructor
    unb::info_allocator<item> b = a;                // copy constructor
    unb::info_allocator<item> c( std::move(b) );    // move constructor

    a = c;                                          // copy assignment
    a = std::move(c);                               // move assignment

    std::allocator<item> d;
    unb::info_allocator<item> e( d );               // value constructor
    unb::info_allocator<item> f( std::move(e) );    // value move constructor

    (void)f;                            // Suppress "unused variable" warning.

    // The allocator must provide sufficient functionality for use with
    // standard containers.
    //std::vector<item, unbuggy::info_allocator<item> > v(a);
}
