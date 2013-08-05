/// \file counting_allocator.tpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#include <utility>  // move

namespace unbuggy {

    // Delegate

    inline
    counting_allocator_delegate::counting_allocator_delegate( ) noexcept
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
      , m_allocated_memory_now( )
      , m_allocated_memory_min( )
      , m_allocated_objects_now( )
      , m_allocated_objects_min( )
      , m_constructed_objects_now( )
      , m_constructed_objects_min( )
    { }

    template <typename A>
    typename std::allocator_traits<A>::pointer
    counting_allocator_delegate::allocate(
            A&                                           a
          , typename std::allocator_traits<A>::size_type n)
    {
        // Calls

        ++m_allocate_calls;

        typename std::allocator_traits<A>::pointer r =
            std::allocator_traits<A>::allocate(a, n);   // may throw

        // All, current, and max objects

        m_allocated_objects_all += n;
        m_allocated_objects_now += n;

        if (m_allocated_objects_now > 0 &&
                std::size_t( m_allocated_objects_now ) >
                m_allocated_objects_max)
            m_allocated_objects_max = m_allocated_objects_now;

        // All, current, and max memory

        std::size_t m =
            n * sizeof(typename std::allocator_traits<A>::value_type);

        m_allocated_memory_all += m;
        m_allocated_memory_now += m;

        if (m_allocated_memory_now > 0 &&
                std::size_t( m_allocated_memory_now ) > m_allocated_memory_max)
            m_allocated_memory_max = m_allocated_memory_now;

        return r;
    }

    template <typename A, typename P>
    typename std::allocator_traits<A>::pointer
    counting_allocator_delegate::allocate(
            A&                                           a
          , typename std::allocator_traits<A>::size_type n
          , P                                            hint)
    {
        // Calls

        ++m_allocate_calls;

        typename std::allocator_traits<A>::pointer r =
            std::allocator_traits<A>::allocate(a, n, hint);     // may throw

        // All, current, and max objects

        m_allocated_objects_all += n;
        m_allocated_objects_now += n;

        if (m_allocated_objects_now > 0 &&
                std::size_t( m_allocated_objects_now ) >
                m_allocated_objects_max)
            m_allocated_objects_max = m_allocated_objects_now;

        // All, current, and max memory

        std::size_t m =
            n * sizeof(typename std::allocator_traits<A>::value_type);

        m_allocated_memory_all += m;
        m_allocated_memory_now += m;

        if (m_allocated_memory_now > 0 &&
                std::size_t( m_allocated_memory_now ) > m_allocated_memory_max)
            m_allocated_memory_max = m_allocated_memory_now;

        return r;
    }

    template <typename A>
    void counting_allocator_delegate::deallocate(
            A&                                           a
          , typename std::allocator_traits<A>::pointer   p
          , typename std::allocator_traits<A>::size_type n) noexcept
    {
        // Calls

        ++m_deallocate_calls;

        std::allocator_traits<A>::deallocate(a, p, n);

        // All, current, and min objects

        m_deallocated_objects_all += n;
        m_allocated_objects_now   -= n;

        if (m_allocated_objects_now < m_allocated_objects_min)
            m_allocated_objects_min = m_allocated_objects_now;

        // All, current, and min memory

        std::size_t m =
            n * sizeof(typename std::allocator_traits<A>::value_type);

        m_deallocated_memory_all += m;
        m_allocated_memory_now   -= m;

        if (m_allocated_memory_now < m_allocated_memory_min)
            m_allocated_memory_min = m_allocated_memory_now;
    }

    template <typename A, typename C, typename... Args>
    void counting_allocator_delegate::construct(A& a, C* c, Args&&... args)
    {
        ++m_construct_calls;

        std::allocator_traits<A>::construct(a, c, args...);

        ++m_constructed_objects_all;

        if (++m_constructed_objects_now > 0 &&
                std::size_t( m_constructed_objects_now ) >
                m_constructed_objects_max)
            m_constructed_objects_max = m_constructed_objects_now;
    }

    template <typename A, typename C>
    void counting_allocator_delegate::destroy(A& a, C* c)
    {
        ++m_destroy_calls;

        std::allocator_traits<A>::destroy(a, c);

        if (--m_constructed_objects_now < m_constructed_objects_min)
            m_constructed_objects_min = m_constructed_objects_now;
    }

    template <typename A>
    typename std::allocator_traits<A>::size_type
    counting_allocator_delegate::max_size(A const& a)
    {
        ++m_max_size_calls;

        return std::allocator_traits<A>::max_size(a);
    }

    template <typename A>
    A counting_allocator_delegate::select_on_container_copy_construction(
            A const& a)
    {
        ++m_select_on_container_copy_construction_calls;

        return A( std::allocator_traits<A>::
                select_on_container_copy_construction(a) );
    }

    inline
    std::size_t counting_allocator_delegate::allocate_calls() const
    {
        return m_allocate_calls;
    }

    inline
    std::size_t counting_allocator_delegate::allocated_objects_all() const
    {
        return m_allocated_objects_all;
    }

    inline
    std::size_t counting_allocator_delegate::allocated_objects_max() const
    {
        return m_allocated_objects_max;
    }

    inline
    std::ptrdiff_t counting_allocator_delegate::allocated_objects_min() const
    {
        return m_allocated_objects_min;
    }

    inline
    std::ptrdiff_t counting_allocator_delegate::allocated_objects_now() const
    {
        return m_allocated_objects_now;
    }

    inline
    std::size_t counting_allocator_delegate::allocated_memory_all() const
    {
        return m_allocated_memory_all;
    }

    inline
    std::size_t counting_allocator_delegate::allocated_memory_max() const
    {
        return m_allocated_memory_max;
    }

    inline
    std::ptrdiff_t counting_allocator_delegate::allocated_memory_min() const
    {
        return m_allocated_memory_min;
    }

    inline
    std::ptrdiff_t counting_allocator_delegate::allocated_memory_now() const
    {
        return m_allocated_memory_now;
    }

    inline
    std::size_t counting_allocator_delegate::deallocate_calls() const
    {
        return m_deallocate_calls;
    }

    inline
    std::size_t counting_allocator_delegate::deallocated_objects_all() const
    {
        return m_deallocated_objects_all;
    }

    inline
    std::size_t counting_allocator_delegate::deallocated_memory_all() const
    {
        return m_deallocated_memory_all;
    }

    inline
    std::size_t counting_allocator_delegate::construct_calls() const
    {
        return m_construct_calls;
    }

    inline
    std::size_t counting_allocator_delegate::constructed_objects_all() const
    {
        return m_constructed_objects_all;
    }

    inline
    std::size_t counting_allocator_delegate::constructed_objects_max() const
    {
        return m_constructed_objects_max;
    }

    inline
    std::ptrdiff_t counting_allocator_delegate::constructed_objects_min() const
    {
        return m_constructed_objects_min;
    }

    inline
    std::ptrdiff_t counting_allocator_delegate::constructed_objects_now() const
    {
        return m_constructed_objects_now;
    }

    inline
    std::size_t counting_allocator_delegate::destroy_calls() const
    {
        return m_destroy_calls;
    }

    inline
    std::size_t counting_allocator_delegate::max_size_calls() const
    {
        return m_max_size_calls;
    }

    inline
    std::size_t
    counting_allocator_delegate::
    select_on_container_copy_construction_calls() const
    {
        return m_select_on_container_copy_construction_calls;
    }

    // Mixin

    template <typename A>
    std::size_t counting_allocator_mixin<A>::allocate_calls() const
    {
        return static_cast<A const*>(this)->delegate().allocate_calls();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::allocated_objects_all() const
    {
        return static_cast<A const*>(this)->delegate().allocated_objects_all();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::allocated_objects_max() const
    {
        return static_cast<A const*>(this)->delegate().allocated_objects_max();
    }

    template <typename A>
    std::ptrdiff_t counting_allocator_mixin<A>::allocated_objects_min() const
    {
        return static_cast<A const*>(this)->delegate().allocated_objects_min();
    }

    template <typename A>
    std::ptrdiff_t counting_allocator_mixin<A>::allocated_objects_now() const
    {
        return static_cast<A const*>(this)->delegate().allocated_objects_now();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::allocated_memory_all() const
    {
        return static_cast<A const*>(this)->delegate().allocated_memory_all();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::allocated_memory_max() const
    {
        return static_cast<A const*>(this)->delegate().allocated_memory_max();
    }

    template <typename A>
    std::ptrdiff_t counting_allocator_mixin<A>::allocated_memory_min() const
    {
        return static_cast<A const*>(this)->delegate().allocated_memory_min();
    }

    template <typename A>
    std::ptrdiff_t counting_allocator_mixin<A>::allocated_memory_now() const
    {
        return static_cast<A const*>(this)->delegate().allocated_memory_now();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::deallocate_calls() const
    {
        return static_cast<A const*>(this)->delegate().deallocate_calls();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::deallocated_objects_all() const
    {
        return static_cast<A const*>(this)->
            delegate().deallocated_objects_all();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::deallocated_memory_all() const
    {
        return static_cast<A const*>(this)->
            delegate().deallocated_memory_all();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::construct_calls() const
    {
        return static_cast<A const*>(this)->delegate().construct_calls();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::constructed_objects_all() const
    {
        return static_cast<A const*>(this)->
            delegate().constructed_objects_all();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::constructed_objects_max() const
    {
        return static_cast<A const*>(this)->
            delegate().constructed_objects_max();
    }

    template <typename A>
    std::ptrdiff_t counting_allocator_mixin<A>::constructed_objects_min() const
    {
        return static_cast<A const*>(this)->
            delegate().constructed_objects_min();
    }

    template <typename A>
    std::ptrdiff_t counting_allocator_mixin<A>::constructed_objects_now() const
    {
        return static_cast<A const*>(this)->
            delegate().constructed_objects_now();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::destroy_calls() const
    {
        return static_cast<A const*>(this)->delegate().destroy_calls();
    }

    template <typename A>
    std::size_t counting_allocator_mixin<A>::max_size_calls() const
    {
        return static_cast<A const*>(this)->delegate().max_size_calls();
    }

    template <typename A>
    std::size_t
    counting_allocator_mixin<A>::
    select_on_container_copy_construction_calls() const
    {
        return static_cast<A const*>(this)->
            delegate().select_on_container_copy_construction_calls();
    }

}   /// \namespace unbuggy
