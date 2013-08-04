/// \file null_allocator_delegate_test.cpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.
///
/// \cond TEST

#include "unbuggy/null_allocator_delegate.hpp"

#include <cassert>      // assert

// A parameter type for delegate method templates.  Side effects in the
// constructor and destructor help distinguish construction from allocation.
//
struct U {
    static int constructor_count;
    static int destructor_count;

    int value;

    U( int v )
      : value( v )
    {
        ++constructor_count;
    }

    ~U()
    {
        ++destructor_count;
    }
};

int U::constructor_count;
int U::destructor_count;

struct test_allocator: std::allocator<int> {

    typedef std::allocator_traits<std::allocator<int> > super_traits;

    typedef typename super_traits::pointer                       pointer;
    typedef typename super_traits::const_pointer           const_pointer;
    typedef typename super_traits::void_pointer             void_pointer;
    typedef typename super_traits::const_void_pointer const_void_pointer;
    typedef typename super_traits::value_type                 value_type;
    typedef typename super_traits::size_type                   size_type;
    typedef typename super_traits::difference_type       difference_type;

    typedef typename super_traits::propagate_on_container_copy_assignment
                                   propagate_on_container_copy_assignment;
    typedef typename super_traits::propagate_on_container_move_assignment
                                   propagate_on_container_move_assignment;
    typedef typename super_traits::propagate_on_container_swap
                                   propagate_on_container_swap;

    test_allocator( std::allocator<int> a =std::allocator<int>( ) );

    size_type invoked_allocate           = 0;
    size_type invoked_allocate_with_hint = 0;
    size_type invoked_deallocate         = 0;
    size_type mutable invoked_max_size   = 0;
    size_type mutable invoked_select     = 0;

    pointer allocate(size_type n);

    template <typename P>
    pointer allocate(size_type n, P hint);

    void deallocate(pointer p, size_type n) noexcept;

    size_type max_size() const;

    template <typename C, typename... Args>
    void construct(C* c, Args&&... args);

    template <typename C>
    void destroy(C* c);

    test_allocator select_on_container_copy_construction() const;
};

test_allocator::test_allocator( std::allocator<int> a )
  : std::allocator<int>( a )
{ }

test_allocator::pointer test_allocator::allocate(size_type n)
{
    invoked_allocate = n;
    return super_traits::allocate(*this, n);
}

template <typename P>
test_allocator::pointer test_allocator::allocate(size_type n, P hint)
{
    invoked_allocate_with_hint = n;
    return super_traits::allocate(*this, n, hint);
}

void test_allocator::deallocate(pointer p, size_type n) noexcept
{
    invoked_deallocate = n;
    super_traits::deallocate(*this, p, n);
}

test_allocator::size_type test_allocator::max_size() const
{
    return invoked_max_size = super_traits::max_size(*this);
}

template <typename C, typename... Args>
void test_allocator::construct(C* c, Args&&... args)
{
    super_traits::construct(*this, c, std::forward<Args>(args)...);
}

template <typename C>
void test_allocator::destroy(C* c)
{
    super_traits::destroy(*this, c);
}

test_allocator test_allocator::select_on_container_copy_construction() const
{
    invoked_select = 1;
    return super_traits::select_on_container_copy_construction(*this);
}

int main()
{
    test_allocator                   a;
    unbuggy::null_allocator_delegate d;
    std::allocator<U>                b;
    std::allocator<U> const&         bc = b;

    U*       c = b.allocate(2);
    U const* h = c;  // hint

    int* p = d.allocate(a, 42);     assert(a.invoked_allocate           == 42);
    int* q = d.allocate(a, 69, h);  assert(a.invoked_allocate_with_hint == 69);
    d.deallocate(a, p, 42);         assert(a.invoked_deallocate         == 42);
    d.deallocate(a, q, 69);         assert(a.invoked_deallocate         == 69);
    auto z = d.max_size(bc);        assert(b.max_size()                 ==  z);
    d.construct(a, c, 2);           assert(U::constructor_count         ==  1);
                                    assert(c->value                     ==  2);
    d.destroy(a, c);                assert(U::destructor_count          ==  1);
    b.deallocate(c, 2);
}

/// \endcond
