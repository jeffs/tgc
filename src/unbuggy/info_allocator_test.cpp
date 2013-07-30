/// @file info_allocator_test.cpp
///
/// @copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.
///
/// @cond

#include "unbuggy/info_allocator.hpp"

#include <cassert>      // assert
#include <type_traits>  // is_same, static_assert

// [allocator.requirements] uses the following terminology:

class T { };                // any non-const, non-reference
class U { };                // object type
class C { };

struct V {                  // a type convertible to T
    operator T() const
    {
        return T( );
    }
};

typedef unbuggy::info_allocator<T> X;
typedef unbuggy::info_allocator<U> Y;

typedef std::allocator_traits<X> XX;
typedef std::allocator_traits<Y> YY;

int main()
{
    // [allocator.requirements] terminology, continued

    T const& t = T( );
    X       a_, a1_, a2_;   // initializers for a, a1, and a2
    X&      a( a_ ), a1( a1_ ), a2( a2_ );
    Y       b;
    C*      c;

    assert(a1 == a);        // prerequisite for initialization of p

    XX::pointer            p = a1.allocate(42);         // size is arbitrary
    XX::const_pointer      q = p;
    XX::void_pointer       w = p;
    XX::const_void_pointer z = q;

    T&                     r = *p;
    T const&               s = *q;
    YY::const_pointer      u = YY::allocate(b, 69);     // size is arbitrary
    V                      v;
    XX::size_type          n = 76;                      // value is arbitrary

    // [allocator.requirements] requires that \c std::allocator_traits<A>
    // define the following types, and that objects of these types support
    // various intuitive syntax.  This test simply checks that the types match
    // those for the underlying standard allocator type.

#define CHECK_TYPE(type)                                            \
    static_assert(                                                  \
            std::is_same<                                           \
                XX::type                                            \
              , std::allocator_traits<std::allocator<T> >::type>( ) \
          , "type " #type " should match decorated allocator traits");

    CHECK_TYPE( pointer            )
    CHECK_TYPE( const_pointer      )
    CHECK_TYPE( void_pointer       )
    CHECK_TYPE( const_void_pointer )
    CHECK_TYPE( value_type         )
    CHECK_TYPE( size_type          )
    CHECK_TYPE( difference_type    )

#undef CHECK_TYPE

    (void)X::const_pointer( X::pointer( ) );
    (void)X::void_pointer(  X::pointer( ) );

    (void)v;                // Suppress unused variable warning.
    (void)c;
    (void)w;
    (void)u;
    (void)n;
    (void)r;
    (void)t;
    (void)s;
    (void)z;

    static_assert(
            std::is_same<
                unbuggy::info_allocator<int>::rebind<double>::other
              , unbuggy::info_allocator<double, std::allocator<double> > >( )
          , "rebind must return the correct allocator type");
}
