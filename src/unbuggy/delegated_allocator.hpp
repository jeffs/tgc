/// \file delegated_allocator.hpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#ifndef UNBUGGY_INCLUDED_DELEGATED_ALLOCATOR
#define UNBUGGY_INCLUDED_DELEGATED_ALLOCATOR

#include <memory>   // allocator, allocator_traits

namespace unbuggy {

/// \cond DETAILS
namespace delegated_allocator_details {

    template <typename D>
    struct shared_delegate;
}
/// \endcond

/// An allocator decorator that forwards all requests to a delegate object.
/// Each \c delegated_allocator owns a private instance of the decorated type,
/// and passes this instance as the first argument of all calls to the
/// delegate.  The delegate is shared by all allocators in the same copy group,
/// including rebound allocators.
///
/// The requirements of the decorated allocator type are enumerated in
/// <code>ISO/IEC 14882:2011 [allocator.requirements]</code>.  The delegate
/// must model the \c AllocatorDelegate concept.
///
/// \param T the allocated type
/// \param D the delegate type
/// \param A the decorated allocator type
///
template <typename T, typename D, typename A =std::allocator<T> >
class delegated_allocator
/// \cond PRIVATE

: A     // Private inheritance enables EBO when A is empty.

/// \endcond
{
    typedef delegated_allocator_details::shared_delegate<D> shared_delegate;

    // Defines allocator and traits types for use with shared_delegate.
    //
    struct delegate_rebind {

        typedef typename std::allocator_traits<A>::
                template rebind_alloc<shared_delegate> alloc;
            // The decorated allocator type, rebound to allocate
            // shared_delegate.

        typedef typename std::allocator_traits<alloc> traits;
            // Standard traits of the rebound allocator type.
    };

    shared_delegate* m_delegate;
        // shared by all allocators in this copy group

    shared_delegate* create_delegate() const;
        // Create and return a shared delegate having reference count 1.

    shared_delegate* create_delegate(D const& d) const;
        // Create and return a copy of \a d, setting the reference count to 1.

    void destroy_delegate(shared_delegate* d) const;
        // Destroy \a d.  The behavior is undefined unless \a d has a reference
        // count of 0.

  public:

    /// forwards the corresponding trait of the decorated allocator type
    ///@{
    typedef typename std::allocator_traits<A>::pointer                 pointer;
    typedef typename std::allocator_traits<A>::const_pointer     const_pointer;
    typedef typename std::allocator_traits<A>::void_pointer       void_pointer;
    typedef typename std::allocator_traits<A>::const_void_pointer
                                                            const_void_pointer;
    typedef typename std::allocator_traits<A>::value_type           value_type;
    typedef typename std::allocator_traits<A>::size_type             size_type;
    typedef typename std::allocator_traits<A>::difference_type difference_type;

    typedef typename std::allocator_traits<A>::
                                        propagate_on_container_copy_assignment
                                        propagate_on_container_copy_assignment;
    typedef typename std::allocator_traits<A>::
                                        propagate_on_container_move_assignment
                                        propagate_on_container_move_assignment;
    typedef typename std::allocator_traits<A>::
                                        propagate_on_container_swap
                                        propagate_on_container_swap;
    ///@}

    // Default Constructor

    delegated_allocator( );
        ///< Decorates a default-constructed instance of \c A.

    // Copy Constructor

    delegated_allocator( delegated_allocator const& original );
        ///< Copies \a original.

    // Move Constructor

    delegated_allocator( delegated_allocator&& original );
        ///< Moves the underlying allocator from \a original.

    // Copy Conversion Constructor

    template <typename U>
    delegated_allocator(
            unbuggy::delegated_allocator<
                U
              , D
              , typename std::allocator_traits<A>::template rebind_alloc<U>
            > const& original);
        ///< Rebinds and decorates the underlying allocator of \a original.

    // Move Conversion Constructor

    template <typename U>
    delegated_allocator(
            unbuggy::delegated_allocator<
                U
              , D
              , typename std::allocator_traits<A>::template rebind_alloc<U>
            >&& original);
        ///< Rebinds and decorates the underlying allocator of \a original.

    // Explicit Constructors

    /// Decorates allocator \a by forwarding requests to delegate \a d.  If \a
    /// d or \a a is not supplied, a default-constructed instance of \a D or \a
    /// A is used, respectively.
    ///@{
    explicit delegated_allocator(             A const& a );
    explicit delegated_allocator(             A&&      a );
    explicit delegated_allocator( D const& d             );
    explicit delegated_allocator( D const& d, A const& a );
    explicit delegated_allocator( D const& d, A&&      a );
    explicit delegated_allocator( D&&      d             );
    explicit delegated_allocator( D&&      d, A const& a );
    explicit delegated_allocator( D&&      d, A&&      a );
    ///@}

    // Destructor

    ~delegated_allocator();
        ///< Destroys the shared delegate if this allocator was the last in its
        ///  copy group.

    // Copy Assignment Operator

    delegated_allocator& operator=(delegated_allocator const& rhs);
        ///< Assigns to this object the value of \a rhs.  This allocator
        ///  leaves its former copy group, and becomes part of the copy group
        ///  of \a rhs.  If this allocator was the last member of its copy
        ///  group, then the group's shared delegate is destroyed.

    // Move Assignment Operator

    delegated_allocator& operator=(delegated_allocator&& rhs);
        ///< Assigns to this object the value of \a rhs.  This allocator
        ///  leaves its former copy group, and becomes part of the copy group
        ///  of \a rhs.  If this allocator was the last member of its copy
        ///  group, then the group's shared delegate is destroyed.  Assignment
        ///  from other instantiations of \c delegated_allocator is supported
        ///  through implicit conversion to this type, followed by invocation
        ///  of this assignment operator.

    // Methods Modeling Allocator

    pointer allocate(size_type n);
        ///< Returns space for \a n objects of type \c T, or throws an
        ///  exception if the space cannot be allocated.

    pointer allocate(size_type n, const_void_pointer hint);
        ///< Returns space for \a n objects of type \c T, passing \a hint to
        ///  the underlying allocator, or throws an exception if the space
        ///  cannot be allocated.

    void deallocate(pointer p, size_type n);
        ///< Frees space for \a n objects beginning at address \a p.  The
        ///  behavior is undefined unless \a p was returned by a previous call
        ///  to \c allocate exactly \a n objects, and has not already been
        ///  deallocated.

    size_type max_size() const;
        ///< Returns the largest value that can meaningfully be passed to \c
        ///  allocate.  Note that \c allocate is not guaranteed to succeed.

    delegated_allocator select_on_container_copy_construction() const;
        ///< Wraps the corresponding function of the underlying allocator.

    template <typename C, typename... Args>
    void construct(C* c, Args&&... args);
        ///< Constructs an object of type \c C at \a c.  Supplies \a args to
        ///  the \c C constructor.

    template <typename C>
    void destroy(C* c);
        ///< Destroys the object at \a c.

    // Other Methods

    A get_allocator() const;
        ///< Returns the decorated allocator.

    D get_delegate() const;
        ///< Returns the shared delegate of this allocator's copy group.
};

template <typename T, typename D, typename A>
bool operator==(
        delegated_allocator<T, D, A> const& a1
      , delegated_allocator<T, D, A> const& a2);
    ///< True if memory from one allocator may be deallocated by the other.

template <typename T, typename D, typename A>
bool operator!=(
        delegated_allocator<T, D, A> const& a1
      , delegated_allocator<T, D, A> const& a2);
    ///< Equivalent to <code>!(a1 == a2)</code>.

template <typename T, typename D, typename A, typename U>
bool operator==(
        delegated_allocator<
            T
          , D
          , A
        > const& a
      , delegated_allocator<
            U
          , D
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& b);
    ///< True if <code>a == X(b)</code>, where \c X is the type of \a a.

template <typename T, typename D, typename A, typename U>
bool operator!=(
        delegated_allocator<
            T
          , D
          , A
        > const& a
      , delegated_allocator<
            U
          , D
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& b);
    ///< Equivalent to <code>!(a == b)</code>.

}  /// \namespace unbuggy


#include "unbuggy/delegated_allocator.tpp"
#endif
