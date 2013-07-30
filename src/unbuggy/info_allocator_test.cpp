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

    // Table 28 - Allocator requirements

    // Expression: X::const_pointer
    (void)X::const_pointer(      X::pointer( ) );

    // Expression: X::void_pointer, Y::void_pointer
    (void)X::void_pointer(       X::pointer( ) );
    static_assert(
            std::is_same<X::void_pointer, Y::void_pointer>( )
          , "X::void_pointer and Y::void_pointer must be the same type.");

    // Expression: X::const_void_pointer, Y::const_void_pointer
    (void)X::const_void_pointer( X::pointer( )       );
    (void)X::const_void_pointer( X::const_pointer( ) );
    (void)X::const_void_pointer( X::void_pointer( )  );
    static_assert(
            std::is_same<X::const_void_pointer, Y::const_void_pointer>( )
          , "X::const_void_pointer and Y::const_void_pointer must be the same"
            " type");

    // Expression: X::value_type
    static_assert(
            std::is_same<X::value_type, T>( )
          , "X::value_type must be identical to T");

    // Expression: X::size_type - sufficiently wide unsigned integer type

    // Expression: X::difference_type - sufficiently wide signed integer type

    // Expression: typename X::template rebind<U>::other
    static_assert(
            std::is_same<typename X::template rebind<U>::other, Y>( )
          , "X::rebind<U> must return Y");

    static_assert(
            std::is_same<decltype(*p), T&>( )
          , "*p must be of type T&");

    static_assert(
            std::is_same<decltype(*q), T const&>( )
          , "*q must be of type const T&");

    static_assert(
            std::is_same<decltype(*q), T const&>( )
          , "*q must be of type const T&");

    // Expression: p->m - has type T::m

    // Expression: q->m - has type T::m

    // Expression: static_cast<X::pointer>(w)
    assert(static_cast<X::pointer>(w) == p);

    // Expression: static_cast<X::const_pointer>(z)
    assert(static_cast<X::const_pointer>(z) == q);

    (void)v;                // Suppress unused variable warning.
    (void)c;
    (void)w;
    (void)u;
    (void)n;
    (void)r;
    (void)t;
    (void)s;
    (void)z;

    YY::deallocate(b, const_cast<YY::pointer>(u), 69);
    a1.deallocate(p, 42);
}
