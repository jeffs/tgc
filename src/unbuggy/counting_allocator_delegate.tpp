/// \file counting_allocator_delegate.tpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#include <utility>  // move

namespace unbuggy {

    inline
    counting_allocator_delegate::counting_allocator_delegate( )
      : m_allocate_calls( )
      , m_allocated_memory_all( )
      , m_allocated_memory_max( )
      , m_allocated_objects_all( )
      , m_allocated_objects_max( )
      , m_construct_calls( )
      , m_constructed_objects_all( )
      , m_constructed_objects_max( )
      , m_deallocate_calls( )
      , m_deallocated_memory_all( )
      , m_deallocated_objects_all( )
      , m_destroy_calls( )
      , m_max_size_calls( )
      , m_select_on_container_copy_construction_calls( )
      , m_allocated_memory( )
      , m_allocated_memory_min( )
      , m_allocated_objects( )
      , m_allocated_objects_min( )
      , m_constructed_objects( )
      , m_constructed_objects_min( )
    { }

    template <typename A>
    typename std::allocator_traits<A>::pointer
    counting_allocator_delegate::allocate(
            A&                                           a
          , typename std::allocator_traits<A>::size_type n)
    {
        return std::allocator_traits<A>::allocate(a, n);
    }

    template <typename A, typename P>
    typename std::allocator_traits<A>::pointer 
    counting_allocator_delegate::allocate(
            A&                                           a
          , typename std::allocator_traits<A>::size_type n
          , P                                            hint)
    {
        return std::allocator_traits<A>::allocate(a, n, hint);
    }

    template <typename A>
    void counting_allocator_delegate::deallocate(
            A&                                           a
          , typename std::allocator_traits<A>::pointer   p
          , typename std::allocator_traits<A>::size_type n) noexcept
    {
        return std::allocator_traits<A>::deallocate(a, p, n);
    }

    template <typename A, typename C, typename... Args>
    void counting_allocator_delegate::construct(A& a, C* c, Args&&... args)
    {
        std::allocator_traits<A>::construct(a, c, args...);
    }

    template <typename A, typename C>
    void counting_allocator_delegate::destroy(A& a, C* c)
    {
        std::allocator_traits<A>::destroy(a, c);
    }

    template <typename A>
    typename std::allocator_traits<A>::size_type
    counting_allocator_delegate::max_size(A const& a)
    {
        return std::allocator_traits<A>::max_size(a);
    }

    template <typename A>
    A counting_allocator_delegate::select_on_container_copy_construction(
            A const& a)
    {
        return A( std::allocator_traits<A>::
                select_on_container_copy_construction(a) );
    }

}   /// \namespace unbuggy