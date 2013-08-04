/// \file null_allocator_delegate.hpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#ifndef UNBUGGY_INCLUDED_NULL_ALLOCATOR_DELEGATE
#define UNBUGGY_INCLUDED_NULL_ALLOCATOR_DELEGATE

#include <memory>   // allocator_traits

namespace unbuggy {

/// Forwards all calls directly to a leading allocator parameter.
///
struct null_allocator_delegate {

    template <typename A>
    static typename std::allocator_traits<A>::pointer allocate(
            A& a
          , typename std::allocator_traits<A>::size_type n);
        ///< Returns the result of <code>a.allocate(n)</code>.

    template <typename A>
    static typename std::allocator_traits<A>::pointer allocate(
            A&                                                    a
          , typename std::allocator_traits<A>::size_type          n
          , typename std::allocator_traits<A>::const_void_pointer hint);
        ///< Returns the result of <code>a.allocate(n, hint)</code>.

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
    static A select_on_container_copy_construction(const A& a);
        ///< Returns the result of
        ///  <code>a.select_on_container_copy_construction()</code>.
};

}   /// \namespace unbuggy

#endif
