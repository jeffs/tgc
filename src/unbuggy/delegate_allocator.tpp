/// \file delegate_allocator.tpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#include <utility>  // move

namespace unbuggy {

/// \cond DETAILS
namespace delegate_allocator_details {

    template <typename D>
    struct shared_delegate: D {
        std::size_t ref_count;  // number of allocators sharing this delegate
    };

}
/// \endcond

// Private Methods

template <typename T, typename D, typename A>
typename delegate_allocator<T, D, A>::shared_delegate*
delegate_allocator<T, D, A>::create_delegate(D const& d) const
{
    typename delegate_rebind::alloc b( static_cast<A const&>(*this) );

    shared_delegate* r = delegate_rebind::traits::allocate(b, 1);
    delegate_rebind::traits::construct(b, r, d);
    r->ref_count = 1;

    return r;
}

template <typename T, typename D, typename A>
typename delegate_allocator<T, D, A>::shared_delegate*
delegate_allocator<T, D, A>::create_delegate() const
{
    typename delegate_rebind::alloc b( static_cast<A const&>(*this) );

    shared_delegate* r = delegate_rebind::traits::allocate(b, 1);
    delegate_rebind::traits::construct(b, r);
    r->ref_count = 1;

    return r;
}

template <typename T, typename D, typename A>
void delegate_allocator<T, D, A>::destroy_delegate(shared_delegate* d) const
{
    typename delegate_rebind::alloc b( static_cast<A const&>(*this) );

    delegate_rebind::traits::destroy(b, d);
    delegate_rebind::traits::deallocate(b, d, 1);
}

// Default Constructor

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator( )
  : A( )
  , m_delegate( create_delegate() )
{ }

// Copy Constructor

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator(
        delegate_allocator const& original ) noexcept
  : A( static_cast<A const&>(original) )
  , m_delegate( original.m_delegate )
{
    ++m_delegate->ref_count;
}

// Move Constructor

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator(
        delegate_allocator&& original ) noexcept
  : A( std::move(static_cast<A&>(original)) )
  , m_delegate( original.m_delegate )
{
    ++m_delegate->ref_count;
}

// Copy Conversion Constructor

template <typename T, typename D, typename A>
template <typename U>
delegate_allocator<T, D, A>::delegate_allocator(
        unbuggy::delegate_allocator<
            U
          , D
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& original ) noexcept
  : A( static_cast<
          typename std::allocator_traits<A>::template rebind_alloc<U> const&
       >(original) )
  , m_delegate( original.m_delegate )
{
    ++m_delegate->ref_count;
}

// Move Conversion Constructor

template <typename T, typename D, typename A>
template <typename U>
delegate_allocator<T, D, A>::delegate_allocator(
        unbuggy::delegate_allocator<
            U
          , D
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        >&& original ) noexcept
  : A( static_cast<
          typename std::allocator_traits<A>::template rebind_alloc<U>&&
       >(original) )
  , m_delegate( original.m_delegate )
{
    ++m_delegate->ref_count;
}

// Explicit Constructors

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator( A const& a ) noexcept
  : A( a )
  , m_delegate( create_delegate() )
{ }

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator( A&& a ) noexcept
  : A( std::move(a) )
  , m_delegate( create_delegate() )
{ }

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator( D const& d )
  : A( )
  , m_delegate( create_delegate(d) )
{ }

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator( D const& d, A const& a )
  : A( a )
  , m_delegate( create_delegate(d) )
{ }

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator( D const& d, A&& a )
  : A( std::move(a) )
  , m_delegate( create_delegate(d) )
{ }

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator( D&& d )
  : A( )
  , m_delegate( create_delegate(std::move(d)) )
{ }

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator( D&& d, A const& a )
  : A( a )
  , m_delegate( create_delegate(std::move(d)) )
{ }

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::delegate_allocator( D&& d, A&& a )
  : A( std::move(a) )
  , m_delegate( create_delegate(std::move(d)) )
{ }

// Destructor

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>::~delegate_allocator()
{
    if (--m_delegate->ref_count == 0)
        destroy_delegate(m_delegate);
}

// Copy Assignment Operator

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>&
delegate_allocator<T, D, A>::operator=(delegate_allocator const& rhs)
{
    // Copy the decorated allocator.

    static_cast<A&>(*this) = static_cast<A const&>(rhs);

    // Share the delegate.

    ++rhs.m_delegate->ref_count;

    if (--m_delegate->ref_count == 0)
        destroy_delegate(m_delegate);

    m_delegate = rhs.m_delegate;
}

// Move Assignment Operator

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>&
delegate_allocator<T, D, A>::operator=(delegate_allocator&& rhs)
{
    // Move the decorated allocator.

    static_cast<A&>(*this) = static_cast<A&&>(rhs);

    // Share the delegate.

    ++rhs.m_delegate->ref_count;

    if (--m_delegate->ref_count == 0)
        destroy_delegate(m_delegate);

    m_delegate = rhs.m_delegate;
}

// Methods Modeling Allocator

template <typename T, typename D, typename A>
typename delegate_allocator<T, D, A>::pointer
delegate_allocator<T, D, A>::allocate(size_type n)
{
    return m_delegate->allocate(static_cast<A&>(*this), n);
}

template <typename T, typename D, typename A>
typename delegate_allocator<T, D, A>::pointer
delegate_allocator<T, D, A>::allocate(size_type n, const_void_pointer u)
{
    return m_delegate->allocate(static_cast<A&>(*this), n, u);
}

template <typename T, typename D, typename A>
void delegate_allocator<T, D, A>::deallocate(pointer p, size_type n) noexcept
{
    m_delegate->deallocate(static_cast<A&>(*this), p, n);
}

template <typename T, typename D, typename A>
typename delegate_allocator<T, D, A>::size_type
delegate_allocator<T, D, A>::max_size() const
{
    return m_delegate->max_size(static_cast<A const&>(*this));
}

template <typename T, typename D, typename A>
template <typename C, typename... Args>
void delegate_allocator<T, D, A>::construct(C* c, Args&&... args)
{
    m_delegate->construct(
            static_cast<A&>(*this)
          , c
          , std::forward<Args>(args)...);
}

template <typename T, typename D, typename A>
template <typename C>
void delegate_allocator<T, D, A>::destroy(C* c)
{
    m_delegate->destroy(static_cast<A&>(*this), c);
}

template <typename T, typename D, typename A>
delegate_allocator<T, D, A>
delegate_allocator<T, D, A>::select_on_container_copy_construction() const
{
    return delegate_allocator(
            m_delegate->select_on_container_copy_construction(
                static_cast<A const&>(*this)) );
}

// Other Methods

template <typename T, typename D, typename A>
A delegate_allocator<T, D, A>::get_allocator() const
{
    return static_cast<A const&>(*this);
}

template <typename T, typename D, typename A>
D delegate_allocator<T, D, A>::get_delegate() const
{
    return static_cast<D const&>(*m_delegate);
}

}  /// \namespace unbuggy

template <typename T, typename D, typename A>
bool unbuggy::operator==(
        delegate_allocator<T, D, A> const& a1
      , delegate_allocator<T, D, A> const& a2) noexcept
{
    return a1.get_allocator() == a2.get_allocator();
}

template <typename T, typename D, typename A>
bool unbuggy::operator!=(
        delegate_allocator<T, D, A> const& a1
      , delegate_allocator<T, D, A> const& a2) noexcept
{
    return !(a1 == a2);
}

template <typename T, typename D, typename A, typename U>
bool unbuggy::operator==(
        delegate_allocator<
            T
          , D
          , A
        > const& a
      , delegate_allocator<
            U
          , D
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& b) noexcept
{
    return a.get_allocator() == b.get_allocator();
}

template <typename T, typename D, typename A, typename U>
bool unbuggy::operator!=(
        delegate_allocator<
            T
          , D
          , A
        > const& a
      , delegate_allocator<
            U
          , D
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& b) noexcept
{
    return !(a == b);
}
