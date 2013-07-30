/// @file info_allocator_test.cpp
///
/// @copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#include "unbuggy/info_allocator.hpp"

#include <type_traits>  // is_same, static_assert

int main()
{

    struct item { };                    // dummy value type for allocator

    typedef unbuggy::info_allocator<item> allocator_t;
                                        // type to be tested

    // Standard allocator requirements:

    static_assert(
            std::is_same<item, allocator_t::value_type>( )
          , "value_type must match parameter T");

    // Expectations not required by the standard:

    allocator_t a;                      // default constructor
    allocator_t b = a;                  // copy constructor
    allocator_t c( std::move(b) );      // move constructor

    std::allocator<item> d;
    allocator_t e( d );                 // value constructor
    allocator_t f( std::move(e) );      // value move constructor

    f = c;                              // copy assignment
    f = std::move(c);                   // move assignment
}
