/** @file info_allocator.hpp */

#ifndef INCLUDED_UNBUGGY_INFO_ALLOCATOR
#define INCLUDED_UNBUGGY_INFO_ALLOCATOR

#include <memory>   // allocator
#include <utility>  // move

/// Unbuggy Software LLC enterprise namespace
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
/// @param T the allocated type
/// @param A the underlying allocator type
///
/// @see INCITS-ISO-IEC-14882-2012 [allocator.requirements]
template <typename T, typename A =std::allocator<T> >
class info_allocator {

    A m_a;                              ///< the underlying allocator

  public:

    typedef T value_type;               ///< the allocated type

    info_allocator( );
        ///< Decorates a default-constructed instance of `A`.

    explicit info_allocator( A const& a );
        ///< Decorates a copy of `a`.

    explicit info_allocator( A&& a );
        ///< Decorates an allocator moved from `a`.
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

}  // close enterprise namespace

#endif
