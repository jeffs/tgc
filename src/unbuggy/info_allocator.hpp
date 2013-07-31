/// \file info_allocator.hpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#ifndef INCLUDED_UNBUGGY_INFO_ALLOCATOR
#define INCLUDED_UNBUGGY_INFO_ALLOCATOR

#include <iostream> // XXX
#include <memory>   // allocator, allocator_traits
#include <utility>  // move

namespace unbuggy {

// Declaration of info_allocator

/// A memory allocator that records simple statistics.  Meets the requirements
/// of an STL-compatible memory allocator by forwarding standard allocator
/// requests to an underlying allocator of user-specified type, optionally
/// copied from an instance supplied at construction.  Additionally,
/// `info_allocator` records the following statistics:
///
/// - number of allocations
/// - number of deallocations
/// - total amount of allocated memory (regardless of whether deallocated)
/// - maximum amount of outstanding memory from this allocator at any time
///
/// \param T the allocated type
/// \param A the underlying allocator type
///
/// \see INCITS-ISO-IEC-14882-2012 [allocator.requirements]
//
/// \todo Optionally annotate allocated memory with debug information.
///
template <typename T, typename A =std::allocator<T> >
class info_allocator: A {

  public:

    typedef std::allocator_traits<A> alloc_traits;
        //< allocator traits of type \c A

    /// Type definitions to match the underlying allocator traits.
    //@{

    typedef typename alloc_traits::pointer                       pointer;
    typedef typename alloc_traits::const_pointer           const_pointer;
    typedef typename alloc_traits::void_pointer             void_pointer;
    typedef typename alloc_traits::const_void_pointer const_void_pointer;
    typedef typename alloc_traits::value_type                 value_type;
    typedef typename alloc_traits::size_type                   size_type;
    typedef typename alloc_traits::difference_type       difference_type;

    typedef typename alloc_traits::propagate_on_container_copy_assignment
                                   propagate_on_container_copy_assignment;
    typedef typename alloc_traits::propagate_on_container_move_assignment
                                   propagate_on_container_move_assignment;
    typedef typename alloc_traits::propagate_on_container_swap
                                   propagate_on_container_swap;
    //@}

    /// Provides a typedef for an \c info_allocator of objects of type \c U.
    ///
    template <typename U>
    struct rebind {
        typedef
            unbuggy::info_allocator<
                U
              , typename std::allocator_traits<A>::template rebind_alloc<U> >
            other;                      ///< rebound allocator type
    };

  private:

    size_type m_allocate_calls;         // number of calls to \c allocate
    size_type m_deallocate_calls;       // number of calls to \c deallocate

    size_type m_count_allocated_all;    // total number of objects allocated
    size_type m_count_allocated_max;    // most simultaneous live objects seen
    size_type m_count_allocated_now;    // number of currently live objects

  public:

    info_allocator( );
        ///< Decorates a default-constructed instance of `A`.

    info_allocator( info_allocator const& a );
        ///< Copies \a a.

    template <typename U>
    info_allocator(
            unbuggy::info_allocator<
                U
              , typename std::allocator_traits<A>::template rebind_alloc<U>
            > const& a);
        ///< Converts and decorates the underlying allocator of \a a.  Note
        /// that \a a has a type similar to this, except that \a a allocates
        /// objects of type \a U rather than type \c T.  Also note that this
        /// conversion constructor is required by the C++ Standard (Table 28,
        /// expression <code>X a(b)</code>), and is implicit (in accordance with
        /// the example in [allocator.requirements] clause 5) so that the
        /// allocator of one container may be conveniently supplied to the
        /// constructor of an allocator having a different element type.

    explicit info_allocator( A const& a );
        ///< Decorates a copy of `a`.

    explicit info_allocator( A&& a );
        ///< Decorates an allocator moved from `a`.

    pointer allocate(size_type n, const_void_pointer u =nullptr);
        ///< Returns space for \a n objects of type \c T, passing \a u as a
        /// hint to the underlying allocator, or throws an exception if the
        /// space cannot be allocated.

    void deallocate(pointer p, size_type n);
        ///< Frees space for \a n objects beginning at address \p.  The
        /// behavior is undefined unless \a p was returned by a previous call
        /// to \c allocate exactly \a n objects.

    size_type max_size() const;
        ///< Returns the largest value that can meaningfully be passed to \c
        /// allocate.  Note that \c allocate is not guaranteed to succeed.

    A& get_allocator();
        ///< Returns the decorated allocator.

    A const& get_allocator() const;
        ///< Returns the decorated allocator.

    info_allocator select_on_container_copy_construction() const;

    size_type count_allocated_now() const;
        ///< Returns the number of currently live objects.
};

template <typename T, typename A>
bool operator==(
        info_allocator<T, A> const& a1
      , info_allocator<T, A> const& a2);
    ///< Returns \c true if \a a1 and \a a2 have the same value.  Objects of
    /// class \c info_allocator have the same value if their decorated
    /// allocators compare equal via \c operator==.  If allocators compare
    /// equal, storage allocated from each may be deallocated by the other.

template <typename T, typename A>
bool operator!=(
        info_allocator<T, A> const& a1
      , info_allocator<T, A> const& a2);
    ///< Returns \c true if \a a1 and \a a2 do not have the same value.
    /// Equivalent to <code>!(a1 == a2)</code>.

template <typename T, typename A, typename U>
bool operator==(
        info_allocator<
            T
          , A
        > const& a
      , info_allocator<
            U
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& b);
    ///< Returns \c true if \a a and \a b have the same value.  The allocators
    /// have the same value if \a a has the same value as the result of
    /// explicitly converting \a b to the type of \a a.  If allocators compare
    /// equal, storage allocated from each may be deallocated by the other.
    /// Note that this comparison operation is required by the C++ Standard.

template <typename T, typename A, typename U>
bool operator!=(
        info_allocator<
            T
          , A
        > const& a
      , info_allocator<
            U
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& b);
    ///< Returns \c true if \a a and \a b do not have the same value.
    /// Equivalent to <code>!(a == b)</code>.

// Definition of info_allocator

template <typename T, typename A>
info_allocator<T, A>::info_allocator( )
  : A( )
  , m_allocate_calls( )
  , m_deallocate_calls( )
  , m_count_allocated_all( )
  , m_count_allocated_max( )
  , m_count_allocated_now( )
{ }

template <typename T, typename A>
info_allocator<T, A>::info_allocator( info_allocator const& a )
  : A( a.get_allocator() )
  , m_allocate_calls( )
  , m_deallocate_calls( )
  , m_count_allocated_all( )
  , m_count_allocated_max( )
  , m_count_allocated_now( )
{ }

template <typename T, typename A>
template <typename U>
info_allocator<T, A>::info_allocator(
        unbuggy::info_allocator<
            U
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& a)
  : A( a.get_allocator() )
  , m_allocate_calls( )
  , m_deallocate_calls( )
  , m_count_allocated_all( )
  , m_count_allocated_max( )
  , m_count_allocated_now( )
{ }

template <typename T, typename A>
info_allocator<T, A>::info_allocator( A const& a )
  : A( a )
  , m_allocate_calls( )
  , m_deallocate_calls( )
  , m_count_allocated_all( )
  , m_count_allocated_max( )
  , m_count_allocated_now( )
{ }

template <typename T, typename A>
info_allocator<T, A>::info_allocator( A&& a )
  : A( std::move(a) )
  , m_allocate_calls( )
  , m_deallocate_calls( )
  , m_count_allocated_all( )
  , m_count_allocated_max( )
  , m_count_allocated_now( )
{ }

template <typename T, typename A>
typename info_allocator<T, A>::pointer
info_allocator<T, A>::allocate(size_type n, const_void_pointer u)
{
    pointer r = A::allocate(n, u);  // may throw

    m_count_allocated_all += n;
    m_count_allocated_now += n;

    if (m_count_allocated_now > m_count_allocated_max)
        m_count_allocated_max = m_count_allocated_now;

    ++m_allocate_calls;

    return r;
}

template <typename T, typename A>
void info_allocator<T, A>::deallocate(pointer p, size_type n)
{
    ++m_deallocate_calls;

    m_count_allocated_now -= n;

    A::deallocate(p, n);   // must not throw
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::max_size() const
{
    return std::allocator_traits<A>::max_size(get_allocator());
}

template <typename T, typename A>
A& info_allocator<T, A>::get_allocator()
{
    return *this;
}

template <typename T, typename A>
A const& info_allocator<T, A>::get_allocator() const
{
    return *this;
}

template <typename T, typename A>
info_allocator<T, A>
info_allocator<T, A>::select_on_container_copy_construction() const
{
    return info_allocator(
            std::allocator_traits<A>::
            select_on_container_copy_construction(get_allocator()) );
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::count_allocated_now() const
{
    return m_count_allocated_now;
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
    typedef typename std::allocator_traits<A>::template rebind_alloc<U> B;
    typedef typename info_allocator<U, B>::template rebind<T>::other    Y;

    return a == Y( b );
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

#endif
