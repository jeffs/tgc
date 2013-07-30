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
/// - amount of allocated (but not de-allocated) memory
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

    typedef typename std::allocator_traits<A> a_traits_t;
            // convenient name for use in later type definitions

    A m_a;  // decorated allocator

  public:

    /// Type definitions to match the underlying allocator traits. 
    //@{
    typedef typename a_traits_t::pointer            pointer;
    typedef typename a_traits_t::const_pointer      const_pointer;
    typedef typename a_traits_t::void_pointer       void_pointer;
    typedef typename a_traits_t::const_void_pointer const_void_pointer;
    typedef typename a_traits_t::value_type         value_type;
    typedef typename a_traits_t::size_type          size_type;
    typedef typename a_traits_t::difference_type    difference_type;
    //@}

    /// A typedef template for \c info_allocator<U>.
    ///
    template <typename U>
    struct rebind {
        typedef unbuggy::info_allocator<U> other;  ///< rebound allocator type
    };

    info_allocator( );
        ///< Decorates a default-constructed instance of `A`.

    explicit info_allocator( A const& a );
        ///< Decorates a copy of `a`.

    explicit info_allocator( A&& a );
        ///< Decorates an allocator moved from `a`.

    pointer allocate(size_type n);
        ///< Returns space for \a n objects of type \c T.
};

template <typename T, typename A>
info_allocator<T, A>::info_allocator( )
  : m_a( )
{ }

template <typename T, typename A>
info_allocator<T, A>::info_allocator( A const& a )
  : m_a( a )
{ }

template <typename T, typename A>
info_allocator<T, A>::info_allocator( A&& a )
  : m_a( std::move(a) )
{ }

}  // \namespace unbuggy

#endif
