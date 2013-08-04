/// \file null_allocator_delegate.tpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#include <utility>  // move

namespace unbuggy {

    template <typename A>
    typename std::allocator_traits<A>::pointer
    null_allocator_delegate::allocate(
            A&                                           a
          , typename std::allocator_traits<A>::size_type n)
    {
        return std::allocator_traits<A>::allocate(a, n);
    }

    template <typename A, typename P>
    typename std::allocator_traits<A>::pointer 
    null_allocator_delegate::allocate(
            A&                                           a
          , typename std::allocator_traits<A>::size_type n
          , P                                            hint)
    {
        return std::allocator_traits<A>::allocate(a, n, hint);
    }

    template <typename A>
    void null_allocator_delegate::deallocate(
            A&                                           a
          , typename std::allocator_traits<A>::pointer   p
          , typename std::allocator_traits<A>::size_type n) noexcept
    {
        return std::allocator_traits<A>::deallocate(a, p, n);
    }

    template <typename A, typename C, typename... Args>
    void null_allocator_delegate::construct(A& a, C* c, Args&&... args)
    {
        std::allocator_traits<A>::construct(a, c, args...);
    }

    template <typename A, typename C>
    void null_allocator_delegate::destroy(A& a, C* c)
    {
        std::allocator_traits<A>::destroy(a, c);
    }

    template <typename A>
    typename std::allocator_traits<A>::size_type
    null_allocator_delegate::max_size(A const& a)
    {
        return std::allocator_traits<A>::max_size(a);
    }

    template <typename A>
    A null_allocator_delegate::select_on_container_copy_construction(
            A const& a)
    {
        return A( std::allocator_traits<A>::
                select_on_container_copy_construction(a) );
    }

}   /// \namespace unbuggy
