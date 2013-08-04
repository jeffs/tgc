/// \file delegated_allocator_test.cpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.
///
/// \cond TEST

#include "unbuggy/delegated_allocator.hpp"

#include "unbuggy/null_allocator_delegate.hpp"

#include <cassert>      // assert
#include <type_traits>  // is_same, static_assert

// The C++ Standard 14882-2012 specifies, in [allocator.requirements], a number
// of requirements of standard allocators.  The requirements are explained
// using several predefined types and variable names.  In the following test
// code, types and variables matching those used in the standard requirements
// are defined, and requirements are tested in procedure
// test_standard_requirements, in the order expressed by the standard.  These
// tests are superficial, as they check only the bare minimum requirements of
// an allocator; further tests follow the standard requirements, in procedure
// test_further_requirements.

// T is any non-const, non-reference object type.  The following implementation
// supports side effects in the constructor and destructor, useful for checking
// whether they are invoked by allocator methods.
//
struct T {
    static int constructor_count;
    static int destructor_count;

    T( )
    {
        ++constructor_count;
    }

    ~T()
    {
        ++destructor_count;
    }
};

int T::constructor_count;
int T::destructor_count;

class U { };                // any non-const, non-reference object type

// C is any non-const, non-reference object type.  The following implementation
// accepts a value in the constructor, useful for checking whether constructor
// parameters are forwarded by allocator construct methods.  Additionally, C is
// derived from T, so that C construction and destruction count toward
// T::constructor_count and T::destructor_count.
//
struct C: T {
    int value;

    C( int v )
     : value( v )
    { }
};

static_assert(sizeof(C) != sizeof(T), "C and T should have different sizes");
    // The value member of C causes C to have a different size from T, making a
    // combination of T and C allocations a simple way to modify the shared
    // state of rebound allocators.

struct V {                  // a type convertible to T
    operator T() const
    {
        return T( );
    }
};

typedef unbuggy::delegated_allocator<T, unbuggy::null_allocator_delegate> X;
typedef unbuggy::delegated_allocator<U, unbuggy::null_allocator_delegate> Y;

typedef std::allocator_traits<X> XX;
typedef std::allocator_traits<Y> YY;

typedef std::allocator_traits<std::allocator<T> > A;
    // convenient for testing delegated_allocator

void test_standard_requirements()
{
    // [allocator.requirements] terminology, continued

    T const& t = T( );
    X       a_, a1_, a2_;   // initializers for a, a1, and a2
    X&      a( a_ ), &a1( a1_ ), &a2( a2_ );
    Y       b;
    C*      c;

    assert(a1 == a);        // prerequisite for initialization of p

    XX::pointer            p = a1.allocate(42);         // size matches n
    XX::const_pointer      q = p;
    XX::void_pointer       w = p;
    XX::const_void_pointer z = q;

    T&                     r = *p;
    T const&               s = *q;
    YY::const_pointer      u = YY::allocate(b, 69);     // size is arbitrary
    V                      v;
    XX::size_type          n = 42;                      // size matches p

    (void)a2;
    (void)c;
    (void)n;
    (void)r;
    (void)s;
    (void)t;
    (void)u;
    (void)v;
    (void)w;
    (void)z;

    // [allocator.requirements] requires that \c std::allocator_traits<A>
    // define the following types, and that objects of these types support
    // various intuitive syntax.  The following code simply checks that the
    // types match those for the underlying standard allocator type.

#define CHECK_TYPE(type)                                            \
    static_assert(                                                  \
            std::is_same<                                           \
                XX::type                                            \
              , std::allocator_traits<std::allocator<T> >::type     \
            >::value                                                \
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
    (void)X::const_pointer( X::pointer( ) );

    // Expression: X::void_pointer, Y::void_pointer
    (void)X::void_pointer( X::pointer( ) );
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

    // Expression: a.allocate(n) - allocates, but does not construct
    static_assert(
            std::is_same<decltype(a.allocate(n)), X::pointer>( )
          , "a.allocate(n) must return X::pointer");
    T::constructor_count = 0;
    T::destructor_count = 0;
    a.allocate(n);
    assert(T::constructor_count == 0);
    assert(T::destructor_count == 0);

    // Expression: a.allocate(n, u)
    static_assert(
            std::is_same<decltype(a.allocate(n, u)), X::pointer>( )
          , "a.allocate(n, u) must return X::pointer");
    a.allocate(n, u);
    assert(T::constructor_count == 0);
    assert(T::destructor_count == 0);

    // Expression: a.deallocate(p, n)
    a.deallocate(p, n);
    assert(T::constructor_count == 0);
    assert(T::destructor_count == 0);

    // Expression: a.max_size()
    static_assert(
            std::is_same<decltype(a.max_size()), X::size_type>( )
          , "a.max_size() must return X::size_type");

    // Expression: a1 == a2
    static_assert(
            std::is_same<decltype(a1 == a2), bool>( )
          , "a1 == a2 must return bool");
    assert(a1 == a2);

    // Expression: a1 != a2
    static_assert(
            std::is_same<decltype(a1 != a2), bool>( )
          , "a1 != a2 must return bool");
    assert((a1 != a2) == !(a1 == a2));

    // Expression: a == b
    static_assert(
            std::is_same<decltype(a == b), bool>( )
          , "a == b must return bool");
    assert(a == b);

    // Expression: a != b
    static_assert(
            std::is_same<decltype(a != b), bool>( )
          , "a != b must return bool");
    assert((a != b) == !(a == b));

    // Expression: X a1(a)
    {
        X a1( a );
        assert(a1 == a);        // post: a1 == a
    }

    // Expression: X a(b)
    {
        X a( b );
        assert(Y( a ) == b);    // post: Y(a) == b, a == X(b)
        assert(a == X( b ));
    }

    // Expression X a1(move(a))
    {
        X a0( a );
        X a1( std::move(a) );
        assert(a1 == a0);       // post: a1 equals the prior value of a
    }

    // Expression: X a(move(b))
    {
        X b0( b );
        X a( std::move(b) );
        assert(a == b0);        // post: a equals the prior value of X(b)
    }

    // Expression: a.construct(c, args)
    c = X::rebind<C>::other( a ).allocate(1);
    XX::construct(a, c, 42);
    assert(T::constructor_count == 1);
    assert(T::destructor_count == 0);
    assert(c->value == 42);

    // Expression: a.destroy(c)
    XX::destroy(a, c);
    assert(T::constructor_count == 1);
    assert(T::destructor_count == 1);

    // Expression: a.select_on_container_copy_construction()
    static_assert(
            std::is_same<
                decltype(a.select_on_container_copy_construction())
              , X
            >::value
          , "a.select_on_container_copy_construction() must return X");
    assert(a.select_on_container_copy_construction() == a);

    // Expression: X::propagate_on_container_copy_assignment
    static_assert(
            std::is_same<
                X::propagate_on_container_copy_assignment
              , A::propagate_on_container_copy_assignment
            >::value
          , "X must expose propagate_on_container_copy_assignment");

    // Expression: X::propagate_on_container_move_assignment
    static_assert(
            std::is_same<
                X::propagate_on_container_move_assignment
              , A::propagate_on_container_move_assignment
            >::value
          , "X must expose propagate_on_container_copy_assignment");

    // Expression: X::propagate_on_container_swap
    static_assert(
            std::is_same<
                X::propagate_on_container_swap
              , A::propagate_on_container_swap
            >::value
          , "X must expose propagate_on_container_swap");

    (void)v;                    // Suppress unused variable warnings.
    (void)w;
    (void)r;
    (void)t;
    (void)s;
    (void)z;
}

void test_further_requirements()
{
#if 0

    // A default-constructed delegated_allocator must default-construct its
    // underlying allocator.

    std::allocator<T>             a0;
    X                             a;        assert(a.get_allocator() == a0);
    unbuggy::delegated_allocator<T, X> b;        assert(b.get_allocator() == a);

    // A value-constructed delegated_allocator must wrap the supplied allocator.

    X a1( a0 );                             assert(a1.get_allocator() == a0);

    // Counts must be correctly maintained for interspersed allocations and
    // deallocations.

    XX::pointer p = XX::allocate(a, 3);     assert(a.allocate_calls()   == 1);
                                            assert(a.deallocate_calls() == 0);
    std::size_t oa = a.objects_all();       assert(oa                   == 3);
    std::size_t om = a.objects_max();       assert(om                   == 3);
    std::size_t on = a.objects_now();       assert(on                   == 3);
    std::size_t z  = sizeof(T);             assert(a.memory_all()  == oa * z);
                                            assert(a.memory_max()  == om * z);
                                            assert(a.memory_now()  == on * z);

    XX::pointer q = XX::allocate(a, 2);     assert(a.allocate_calls()   == 2);
                                            assert(a.deallocate_calls() == 0);
    oa = a.objects_all();                   assert(oa                   == 5);
    om = a.objects_max();                   assert(om                   == 5);
    on = a.objects_now();                   assert(on                   == 5);
                                            assert(a.memory_all()  == oa * z);
                                            assert(a.memory_max()  == om * z);
                                            assert(a.memory_now()  == on * z);

    XX::deallocate(a, p, 3);                assert(a.allocate_calls()   == 2);
                                            assert(a.deallocate_calls() == 1);
    oa = a.objects_all();                   assert(oa                   == 5);
    om = a.objects_max();                   assert(om                   == 5);
    on = a.objects_now();                   assert(on                   == 2);
                                            assert(a.memory_all()  == oa * z);
                                            assert(a.memory_max()  == om * z);
                                            assert(a.memory_now()  == on * z);

    p = XX::allocate(a, 1);                 assert(a.allocate_calls()   == 3);
                                            assert(a.deallocate_calls() == 1);
    oa = a.objects_all();                   assert(oa                   == 6);
    om = a.objects_max();                   assert(om                   == 5);
    on = a.objects_now();                   assert(on                   == 3);
                                            assert(a.memory_all()  == oa * z);
                                            assert(a.memory_max()  == om * z);
                                            assert(a.memory_now()  == on * z);

    XX::deallocate(a, p, 1);                assert(a.allocate_calls()   == 3);
                                            assert(a.deallocate_calls() == 2);
    oa = a.objects_all();                   assert(oa                   == 6);
    om = a.objects_max();                   assert(om                   == 5);
    on = a.objects_now();                   assert(on                   == 2);
                                            assert(a.memory_all()  == oa * z);
                                            assert(a.memory_max()  == om * z);
                                            assert(a.memory_now()  == on * z);

    // Allocators in the same copy group must share counts, even when rebound.

    typedef unbuggy::delegated_allocator<C> Z;
    typedef std::allocator_traits<Z>   ZZ;

    Z c( a );
    ZZ::pointer r = ZZ::allocate(c, 4);     assert(a.allocate_calls()   ==  4);
                                            assert(a.deallocate_calls() ==  2);
                                            assert(a.objects_all()      == 10);
                                            assert(a.objects_max()      ==  6);
    std::size_t z1 = sizeof(C);             assert(a.objects_now()      ==  6);
    std::size_t ma = 6 * z + 4 * z1;        assert(a.memory_all()       == ma);
    std::size_t mm = 2 * z + 4 * z1;        assert(a.memory_max()       == mm);
    std::size_t mn = 2 * z + 4 * z1;        assert(a.memory_now()       == mn);

    XX::deallocate(a, q, 2);                assert(a.allocate_calls()   ==  4);
                                            assert(a.deallocate_calls() ==  3);
                                            assert(a.objects_all()      == 10);
                                            assert(a.objects_max()      ==  6);
                                            assert(a.objects_now()      ==  4);
    ma = 6 * z + 4 * z1;                    assert(a.memory_all()       == ma);
    mm = 2 * z + 4 * z1;                    assert(a.memory_max()       == mm);
    mn =         4 * z1;                    assert(a.memory_now()       == mn);

    ZZ::deallocate(c, r, 4);                assert(a.allocate_calls()   ==  4);
                                            assert(a.deallocate_calls() ==  4);
                                            assert(a.objects_all()      == 10);
                                            assert(a.objects_max()      ==  6);
                                            assert(a.objects_now()      ==  0);
    ma = 6 * z + 4 * z1;                    assert(a.memory_all()       == ma);
    mm = 2 * z + 4 * z1;                    assert(a.memory_max()       == mm);
    mn =              0;                    assert(a.memory_now()       == mn);

#endif
}

int main()
{
    test_standard_requirements();
    //test_further_requirements();
}

/// \endcond
