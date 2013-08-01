/// \file info_allocator.tpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#include <cassert>  // assert
#include <utility>  // move

namespace unbuggy {

/// \cond DETAILS

namespace info_allocator_details {

// Statistics shared by allocators in a copy group.
//
template <typename Size_type>
struct shared_state {
    Size_type ref_count;              // number of allocators sharing state
    Size_type allocate_calls;         // number of calls to \c allocate
    Size_type deallocate_calls;       // number of calls to \c deallocate
    Size_type objects_all;            // total number of objects allocated
    Size_type objects_max;            // most simultaneous live objects seen
    Size_type objects_now;            // number of currently live objects
    Size_type memory_all;             // total amount of memory allocated
    Size_type memory_max;             // highest amount of live memory yet
    Size_type memory_now;             // amount of currently live memory
};

}  // namespace info_allocator_details

/// \endcond

template <typename T, typename A>
typename info_allocator<T, A>::shared_state*
info_allocator<T, A>::create_shared_state() const
{
    typedef typename a_traits_t::template rebind_traits<shared_state>
        b_traits_t;

    typename a_traits_t::template rebind_alloc<shared_state>
        b( static_cast<A const&>(*this) );

    shared_state* r = b_traits_t::allocate(b, 1);
    b_traits_t::construct(b, r);  // zero-initializes all counts
    r->ref_count = 1;

    return r;
}

template <typename T, typename A>
void info_allocator<T, A>::destroy_shared_state(shared_state* s) const
{
    typedef typename a_traits_t::template rebind_traits<shared_state>
        b_traits_t;

    typename a_traits_t::template rebind_alloc<shared_state>
        b( static_cast<A const&>(*this) );

    b_traits_t::destroy(b, s);
    b_traits_t::deallocate(b, s, 1);
}

template <typename T, typename A>
info_allocator<T, A>::info_allocator( )
  : A( )
  , m_shared( create_shared_state() )
{ }

template <typename T, typename A>
info_allocator<T, A>::info_allocator( info_allocator const& original )
  : A( static_cast<A const&>(original) )
  , m_shared( original.m_shared )
{
    ++m_shared->ref_count;
}

template <typename T, typename A>
info_allocator<T, A>::info_allocator( info_allocator&& original )
  : A( std::move(static_cast<A&>(original)) )
  , m_shared( original.m_shared )
{
    ++m_shared->ref_count;
}

template <typename T, typename A>
template <typename U>
info_allocator<T, A>::info_allocator(
        unbuggy::info_allocator<
            U
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& original)
  : A( static_cast<
          typename std::allocator_traits<A>::template rebind_alloc<U> const&
       >(original) )
  , m_shared( original.m_shared )
{
    ++m_shared->ref_count;
}

template <typename T, typename A>
info_allocator<T, A>::info_allocator( A const& a )
  : A( a )
  , m_shared( create_shared_state() )
{ }

template <typename T, typename A>
info_allocator<T, A>::info_allocator( A&& a )
  : A( std::move(a) )
  , m_shared( create_shared_state() )
{ }

template <typename T, typename A>
info_allocator<T, A>::~info_allocator()
{
    if (--m_shared->ref_count == 0)
        destroy_shared_state(m_shared);
}

template <typename T, typename A>
info_allocator<T, A>&
info_allocator<T, A>::operator=(info_allocator const& rhs)
{
    static_cast<A&>(*this) = static_cast<A const&>(rhs);
    ++rhs.m_shared->ref_count;
    if (--m_shared->ref_count == 0)
        destroy_shared_state(m_shared);
    m_shared = rhs.m_shared;
}

template <typename T, typename A>
info_allocator<T, A>&
info_allocator<T, A>::operator=(info_allocator&& rhs)
{
    static_cast<A&>(*this) = std::move(static_cast<A const&>(rhs));
    ++rhs.m_shared->ref_count;
    if (--m_shared->ref_count == 0)
        destroy_shared_state(m_shared);
    m_shared = rhs.m_shared;
}

template <typename T, typename A>
typename info_allocator<T, A>::pointer
info_allocator<T, A>::allocate(size_type n, const_void_pointer u)
{
    pointer r = A::allocate(n, u);  // may throw

    ++m_shared->allocate_calls;
    m_shared->objects_all += n;
    m_shared->objects_now += n;

    if (m_shared->objects_now > m_shared->objects_max)
        m_shared->objects_max = m_shared->objects_now;

    m_shared->memory_all += n * sizeof(T);
    m_shared->memory_now += n * sizeof(T);

    if (m_shared->memory_now > m_shared->memory_max)
        m_shared->memory_max = m_shared->memory_now;

    return r;
}

template <typename T, typename A>
void info_allocator<T, A>::deallocate(pointer p, size_type n)
{
    assert(m_shared->memory_now  >= n * sizeof(T));
    assert(m_shared->objects_now >= n);

    m_shared->memory_now  -= n * sizeof(T);
    m_shared->objects_now -= n;
    ++m_shared->deallocate_calls;

    A::deallocate(p, n);   // must not throw
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::max_size() const
{
    return a_traits_t::max_size(static_cast<A const&>(*this));
}

template <typename T, typename A>
A info_allocator<T, A>::get_allocator() const
{
    return static_cast<A const&>(*this);
}

template <typename T, typename A>
info_allocator<T, A>
info_allocator<T, A>::select_on_container_copy_construction() const
{
    return info_allocator(
            a_traits_t::select_on_container_copy_construction(
                static_cast<A const&>(*this)) );
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::allocate_calls() const
{
    return m_shared->allocate_calls;
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::deallocate_calls() const
{
    return m_shared->deallocate_calls;
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::objects_all() const
{
    return m_shared->objects_all;
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::objects_max() const
{
    return m_shared->objects_max;
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::objects_now() const
{
    return m_shared->objects_now;
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::memory_all() const
{
    return m_shared->memory_all;
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::memory_max() const
{
    return m_shared->memory_max;
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::memory_now() const
{
    return m_shared->memory_now;
}

}  /// \namespace unbuggy

template <typename T, typename A>
bool unbuggy::operator==(
        info_allocator<T, A> const& a1
      , info_allocator<T, A> const& a2)
{
    return a1.get_allocator() == a2.get_allocator();
}

template <typename T, typename A>
bool unbuggy::operator!=(
        info_allocator<T, A> const& a1
      , info_allocator<T, A> const& a2)
{
    return !(a1 == a2);
}

template <typename T, typename A, typename U>
bool unbuggy::operator==(
        info_allocator<
            T
          , A
        > const& a
      , info_allocator<
            U
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& b)
{
    return a.get_allocator() == b.get_allocator();
}

template <typename T, typename A, typename U>
bool unbuggy::operator!=(
        info_allocator<
            T
          , A
        > const& a
      , info_allocator<
            U
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& b)
{
    return !(a == b);
}
