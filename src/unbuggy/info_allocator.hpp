/// \file info_allocator.hpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#ifndef INCLUDED_UNBUGGY_INFO_ALLOCATOR
#define INCLUDED_UNBUGGY_INFO_ALLOCATOR

#include <memory>   // allocator, allocator_traits
#include <utility>  // move

namespace unbuggy {

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

    explicit info_allocator( A const& a );
        ///< Decorates a copy of `a`.

    explicit info_allocator( A&& a );
        ///< Decorates an allocator moved from `a`.

    pointer allocate(size_type n, const_pointer u =nullptr);
        ///< Returns space for \a n objects of type \c T, passing \a u as a
        /// hint to the underlying allocator, or throws an exception if the
        /// space cannot be allocated.

    void deallocate(pointer p, size_type n);
        ///< Frees space for \a n objects beginning at address \p.  The
        /// behavior is undefined unless \a p was returned by a previous call
        /// to \c allocate exactly \a n objects.

    size_type max_size() const;
        /// Returns the largest value that can meaningfully be passed to \c
        /// allocate.  Note that \c allocate is not guaranteed to succeed.
};

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
info_allocator<T, A>::allocate(size_type n, const_pointer u)
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

#endif
