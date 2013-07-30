/// \file info_allocator.hpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#ifndef INCLUDED_UNBUGGY_INFO_ALLOCATOR
#define INCLUDED_UNBUGGY_INFO_ALLOCATOR

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
class info_allocator {

  public:

    /// Type definitions to match the underlying allocator traits.
    //@{
    typedef typename std::allocator_traits<A>::pointer                 pointer;
    typedef typename std::allocator_traits<A>::const_pointer     const_pointer;
    typedef typename std::allocator_traits<A>::void_pointer       void_pointer;
    typedef typename std::allocator_traits<A>::const_void_pointer
                                                            const_void_pointer;
    typedef typename std::allocator_traits<A>::value_type           value_type;
    typedef typename std::allocator_traits<A>::size_type             size_type;
    typedef typename std::allocator_traits<A>::difference_type difference_type;
    //@}

    /// Provides a typedef for an \c info_allocator of objects of type \c U.
    ///
    template <typename U>
    struct rebind {
        typedef
            unbuggy::info_allocator<U, typename A::template rebind<U>::other>
            other;                      ///< rebound allocator type
    };

  private:

    A m_a;                              // decorated allocator

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
    explicit info_allocator(
            unbuggy::info_allocator<
                U
              , typename A::template rebind<U>::other
            > const& a);
        ///< Converts and decorates the underlying allocator of \a a.  Note
        /// that \a a has a type similar to this, except that \a a allocates
        /// objects of type \a U rather than type \c T.  Also note that this
        /// conversion constructor is required by the C++ Standard.

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

    A& alloc();
        ///< Returns the decorated allocator.

    A const& alloc() const;
        ///< Returns the decorated allocator.
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
        info_allocator<T, A> const&                                     a
      , info_allocator<U, typename A::template rebind<U>::other> const& b);
    ///< Returns \c true if \a a and \a b have the same value.  The allocators
    /// have the same value if \a a has the same value as the result of
    /// explicitly converting \a b to the type of \a a.  If allocators compare
    /// equal, storage allocated from each may be deallocated by the other.
    /// Note that this comparison operation is required by the C++ Standard.

template <typename T, typename A, typename U>
bool operator!=(
        info_allocator<T, A> const& a
      , info_allocator<U, typename A::template rebind<U>::other> const& b);
    ///< Returns \c true if \a a and \a b do not have the same value.
    /// Equivalent to <code>!(a == b)</code>.

// Definition of info_allocator

template <typename T, typename A>
info_allocator<T, A>::info_allocator( )
  : m_a( )
  , m_allocate_calls( )
  , m_deallocate_calls( )
  , m_count_allocated_all( )
  , m_count_allocated_max( )
  , m_count_allocated_now( )
{ }

template <typename T, typename A>
info_allocator<T, A>::info_allocator( info_allocator const& a )
  : m_a( a.m_a )
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
          , typename A::template rebind<U>::other
        > const& a)
  : m_a( a.alloc() )
  , m_allocate_calls( )
  , m_deallocate_calls( )
  , m_count_allocated_all( )
  , m_count_allocated_max( )
  , m_count_allocated_now( )
{ }

template <typename T, typename A>
info_allocator<T, A>::info_allocator( A const& a )
  : m_a( a )
  , m_allocate_calls( )
  , m_deallocate_calls( )
  , m_count_allocated_all( )
  , m_count_allocated_max( )
  , m_count_allocated_now( )
{ }

template <typename T, typename A>
info_allocator<T, A>::info_allocator( A&& a )
  : m_a( std::move(a) )
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
    pointer r = m_a.allocate(n, u);  // may throw

    m_count_allocated_all += n;

    if (m_count_allocated_now > m_count_allocated_max)
        m_count_allocated_max = m_count_allocated_now;

    m_count_allocated_now += n;

    ++m_allocate_calls;

    return r;
}

template <typename T, typename A>
void info_allocator<T, A>::deallocate(pointer p, size_type n)
{
    ++m_deallocate_calls;

    m_count_allocated_now -= n;

    m_a.deallocate(p, n);   // must not throw
}

template <typename T, typename A>
typename info_allocator<T, A>::size_type
info_allocator<T, A>::max_size() const
{
    return m_a.max_size();
}

}  /// \namespace unbuggy

template <typename T, typename A>
bool unbuggy::operator==(
        info_allocator<T, A> const& a1
      , info_allocator<T, A> const& a2)
{
    return a1.alloc() == a2.alloc();
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
        info_allocator<T, A> const&                                  a
      , info_allocator<U, typename A::template rebind<U>::other> const& b)
{
    return a == typename info_allocator<
        U
      , typename A::template rebind<U>::other
    >::template rebind<T>::other( b );
}

template <typename T, typename A, typename U>
bool unbuggy::operator!=(
        info_allocator<T, A> const&                                  a
      , info_allocator<U, typename A::template rebind<U>::other> const& b)
{
    return !(a == b);
}

template <typename T, typename A>
A& unbuggy::info_allocator<T, A>::alloc()
{
    return m_a;
}

template <typename T, typename A>
A const& unbuggy::info_allocator<T, A>::alloc() const
{
    return m_a;
}

#endif
