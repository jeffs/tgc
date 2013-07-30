/// @file info_allocator_test.cpp
///
/// @copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.
///
/// @cond

#include "unbuggy/info_allocator.hpp"

#include <type_traits>  // is_same, static_assert

int main()
{

    struct item { };                    // dummy value type for allocator

    typedef unbuggy::info_allocator<item> allocator_t;
                                        // type to be tested

    typedef std::allocator_traits<allocator_t> traits_t;
                                        // traits of tested allocator type

    // Expectations not required by the standard:

    allocator_t a;                      // default constructor
    allocator_t b = a;                  // copy constructor
    allocator_t c( std::move(b) );      // move constructor

    std::allocator<item> d;
    allocator_t e( d );                 // value constructor
    allocator_t f( std::move(e) );      // value move constructor

    f = c;                              // copy assignment
    f = std::move(c);                   // move assignment

    // Standard allocator requirements:

#define CHECK_TYPE(t)                                               \
    static_assert(                                                  \
            std::is_same<                                           \
                allocator_t::t                                      \
              , std::allocator_traits<std::allocator<item> >::t>( ) \
          , "type " #t " should match decorated allocator traits");

    CHECK_TYPE( pointer            );
    CHECK_TYPE( const_pointer      );
    CHECK_TYPE( void_pointer       );
    CHECK_TYPE( const_void_pointer );
    CHECK_TYPE( value_type         );
    CHECK_TYPE( size_type          );
    CHECK_TYPE( difference_type    );

#undef CHECK_TYPE

    traits_t::size_type n = 42;                 // arbitrary number of objects

    traits_t::pointer p = a.allocate(n);                // a.allocate(n);
    p = a.allocate(n, traits_t::const_pointer( p ));    // a.allocate(n, u);

}
