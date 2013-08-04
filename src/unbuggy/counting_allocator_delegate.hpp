/// \file counting_allocator_delegate.hpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#ifndef UNBUGGY_INCLUDED_COUNTING_ALLOCATOR_DELEGATE
#define UNBUGGY_INCLUDED_COUNTING_ALLOCATOR_DELEGATE

#include <cstddef>  // ptrdiff_t, size_t
#include <memory>   // allocator_traits

namespace unbuggy {

/// Counts standard method calls, and object and memory allocations.  In method
/// contracts, the term \e live is used to indicate allocations that exceed the
/// number of deallocations, and object constructions exceed the number of
/// destructions.  Note that \e live does not here imply that objects have not
/// been destroyed; rather, it indicates the difference between the number of
/// constructions and destructions performed by a single delegate.
///
class counting_allocator_delegate {

    std::size_t    m_allocate_calls;
    std::size_t    m_allocated_memory_all;
    std::size_t    m_allocated_memory_max;
    std::size_t    m_allocated_objects_all;
    std::size_t    m_allocated_objects_max;
    std::size_t    m_construct_calls;
    std::size_t    m_constructed_objects_all;
    std::size_t    m_constructed_objects_max;
    std::size_t    m_deallocate_calls;
    std::size_t    m_deallocated_memory_all;
    std::size_t    m_deallocated_objects_all;
    std::size_t    m_destroy_calls;
    std::size_t    m_max_size_calls;
    std::size_t    m_select_on_container_copy_construction_calls;
    std::ptrdiff_t m_allocated_memory;
    std::ptrdiff_t m_allocated_memory_min;
    std::ptrdiff_t m_allocated_objects;
    std::ptrdiff_t m_allocated_objects_min;
    std::ptrdiff_t m_constructed_objects;
    std::ptrdiff_t m_constructed_objects_min;

  public:

    // Default Constructor

    counting_allocator_delegate( );
        ///< Initializes all counts to zero.

    // Copy Constructor         = default
    // Move Constructor         = default
    // Destructor               = default
    // Copy Assignment Operator = default
    // Move Assignment Operator = default

    // Methods Modeling AllocatorDelegate

    template <typename A>
    static typename std::allocator_traits<A>::pointer allocate(
            A& a
          , typename std::allocator_traits<A>::size_type n);
        ///< Returns the result of <code>a.allocate(n)</code>.

    template <typename A, typename P>
    static typename std::allocator_traits<A>::pointer allocate(
            A&                                                    a
          , typename std::allocator_traits<A>::size_type          n
          , P                                                     hint);
        ///< Returns the result of <code>a.allocate(n, hint)</code>.  \c P is
        /// \c const_pointer type of (\c A or) some rebind of \c A.

    template <typename A>
    static void deallocate(
            A&                                           a
          , typename std::allocator_traits<A>::pointer   p
          , typename std::allocator_traits<A>::size_type n) noexcept;
        ///< Returns the result of <code>a.deallocate(p, n)</code>.

    template <typename A, typename C, typename... Args>
    static void construct(A& a, C* c, Args&&... args);
        ///< Returns the result of <code>a.construct(c, args...)</code>.

    template <typename A, typename C>
    static void destroy(A& a, C* c);
        ///< Returns the result of <code>a.destroy(c)</code>.

    template <typename A>
    static typename std::allocator_traits<A>::size_type max_size(A const& a);
        ///< Returns the result of <code>a.max_size()</code>.

    template <typename A>
    static A select_on_container_copy_construction(A const& a);
        ///< Decorates the result of
        ///  <code>a.select_on_container_copy_construction()</code>.
        ///  If the result of that method is equal to 

    // Other Methods

    std::size_t allocate_calls() const;
        ///< Returns the number of times \c allocate has been called.  Includes
        ///  calls both with and without hints.

    std::size_t allocated_objects_all() const;
        ///< Returns the total number of allocated objects.  Counts objects
        ///  regardless of whether they have been deallocated.

    std::size_t allocated_objects_max() const;
        ///< Returns the highest number of live allocated objects seen.

    std::ptrdiff_t allocated_objects_min() const;
        ///< Returns the lowest number of live allocated objects seen.  The
        ///  result is negative if, at any time, more objects had been
        ///  deallocated than allocated.

    std::ptrdiff_t allocated_objects() const;
        ///< Returns the current number of live allocated objects.  The result
        ///  is negative if more objects have been deallocated than allocated.

    std::size_t allocated_memory_all() const;
        ///< Returns the total amount of allocated memory.  Counts memory
        ///  regardless of whether it has been deallocated.

    std::size_t allocated_memory_max() const;
        ///< Returns the highest amount of live allocated memory seen.

    std::ptrdiff_t allocated_memory_min() const;
        ///< Returns the lowest amount of live allocated memory seen.  The
        ///  result is negative if, at any time, more memory had been
        ///  deallocated than allocated.

    std::ptrdiff_t allocated_memory() const;
        ///< Returns the current amount of live allocated memory.  The result
        ///  is negative if more memory has been deallocated than allocated.

    std::size_t deallocate_calls() const;
        ///< Returns the number of times \c deallocate has been called.

    std::size_t deallocated_objects_all() const;
        ///< Returns the total number of deallocated objects.

    std::size_t deallocated_memory_all() const;
        ///< Returns the total amount of deallocated memory.

    std::size_t construct_calls() const;
        ///< Returns the number of times \c construct has been called.

    std::size_t constructed_objects_all() const;
        ///< Returns the total number of objects constructed.  The result is
        ///  independent of the number of destroyed objects.  Note that the
        ///  result matches \c construct_calls unless a call to \c construct
        ///  has been unsuccessful (i.e., has thrown an exception).

    std::size_t constructed_objects_max() const;
        ///< Returns the highest number of live constructed objects seen.

    std::ptrdiff_t constructed_objects_min() const;
        ///< Returns the lowest number of live constructed objects seen.  The
        ///  result is negative if, at any time, more objects had been
        ///  destroyed than constructed.

    std::ptrdiff_t constructed_objects() const;
        ///< Returns the current number of live constructed objects.  The
        ///  result is negative if more objects have been destroyed than
        ///  constructed.

    std::size_t destroy_calls() const;
        ///< Returns the number of times \c destroy has been called.  Note that
        ///  the result also indicates the total number of objects destroyed,
        ///  unless an object destructor has thrown an exception.

    std::size_t max_size_calls() const;
        ///< Returns the number of times \c max_size has been called.

    std::size_t select_on_container_copy_construction_calls() const;
        ///< Returns the number of times \c
        ///  select_on_container_copy_construction_calls has been called.

};

}   /// \namespace unbuggy

#include "unbuggy/counting_allocator_delegate.tpp"
#endif
