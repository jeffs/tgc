/// \file info_allocator.hpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#ifndef INCLUDED_UNBUGGY_INFO_ALLOCATOR
#define INCLUDED_UNBUGGY_INFO_ALLOCATOR

#include <memory>   // allocator, allocator_traits

/// \cond DETAILS

namespace unbuggy {

namespace info_allocator_details {

template <typename Size_type>
struct shared_state;

}  // namespace info_allocator_details

/// \endcond

/// A memory allocator that records simple statistics.  Meets the requirements
/// of an STL-compatible memory allocator by forwarding standard allocator
/// requests to an underlying allocator of user-specified type, optionally
/// copied from an instance supplied at construction.  Additionally, \c
/// info_allocator records the following statistics:
///
/// - number of allocations
/// - number of deallocations
/// - total number of allocated objects
/// - number of objects currently live (i.e., not yet deallocated)
/// - maximum number of objects live at any time
/// - total amount of memory ever allocated (regardless of whether deallocated)
/// - amount of memory currently allocated (but not yet deallocated)
/// - maximum amount of allocated memory live at any time
///
/// Statistics are shared by all copies of an \c info_allocator object
/// (including rebound conversions).  For example, if <code>info_allocator
/// b</code> is a copy of <code>info_allocator a</code>, all allocations from
/// either allocator will be reflected in the counts returned by the other.
/// Shared state remains valid until the last allocator in the copy group is
/// destroyed (or assigned a new value); the first instance need not be kept
/// alive simply to maintain statistics.
///
/// Memory consumption is measured as the sum of the sizes of all allocated
/// objects.  Statistics do not include allocations for internal use by \c
/// info_allocator or the underlying allocator.  Internal memory use of an \c
/// info_allocator may be tracked using a second \c info_allocator as the
/// underlying allocator of the first.
///
/// \param T the allocated type
/// \param A the underlying allocator type
///
/// \see INCITS-ISO-IEC-14882-2012 [allocator.requirements]
//
/// \todo Optionally annotate allocated memory with debug information.
///
template <typename T, typename A =std::allocator<T> >
class info_allocator: A {

    typedef std::allocator_traits<A> a_traits_t;
        // for brevity in later type definitions

  public:

    ///@{
    /// matches the underlying allocator traits
    typedef typename a_traits_t::pointer                        pointer;
    typedef typename a_traits_t::const_pointer            const_pointer;
    typedef typename a_traits_t::void_pointer              void_pointer;
    typedef typename a_traits_t::const_void_pointer  const_void_pointer;
    typedef typename a_traits_t::value_type                  value_type;
    typedef typename a_traits_t::size_type                    size_type;
    typedef typename a_traits_t::difference_type        difference_type;

    typedef typename a_traits_t::propagate_on_container_copy_assignment
                                 propagate_on_container_copy_assignment;
    typedef typename a_traits_t::propagate_on_container_move_assignment
                                 propagate_on_container_move_assignment;
    typedef typename a_traits_t::propagate_on_container_swap
                                 propagate_on_container_swap;
    ///@}

    /// Provides a typedef for an \c info_allocator of objects of type \c U.
    ///
    template <typename U>
    struct rebind {
        typedef
            unbuggy::info_allocator<
                U
              , typename std::allocator_traits<A>::template rebind_alloc<U> >
            other;                      ///< rebound allocator type
    };

  private:

    typedef info_allocator_details::shared_state<size_type> shared_state;
        ///< for brevity in later code

    template <typename U, typename B>
    friend class unbuggy::info_allocator;

    shared_state* m_shared;
        ///< statistics shared with copies of this allocator

    shared_state* create_shared_state() const;
        ///< Creates and returns a \c shared_state structure.  The structure is
        /// allocated from a rebind of this object's underlying allocator.  The
        /// result has reference count 1 and all other counts set to 0.

    void destroy_shared_state(shared_state* s) const;
        ///< Destroys and deallocates \a s.  \a s is deallocated by this
        /// object's underlying allocator.

  public:

    info_allocator( );
        ///< Decorates a default-constructed instance of \c A.

    info_allocator( info_allocator const& original );
        ///< Copies \a original.

    info_allocator( info_allocator&& original );
        ///< Moves the underlying allocator from \a original.

    template <typename U>
    info_allocator(
            unbuggy::info_allocator<
                U
              , typename std::allocator_traits<A>::template rebind_alloc<U>
            > const& original);
        ///< Decorates a rebound copy of the underlying allocator from \a
        ///  original Allocation counts and other statistics are shared with \a
        ///  original.  This conversion constructor is required by the C++
        ///  Standard (Table 28, expression <code>X a(b)</code>), and is
        ///  implicit (in accordance with the example in
        ///  [allocator.requirements] clause 5) so that the allocator of one
        ///  container may be conveniently supplied to the constructor of an
        ///  allocator having a different element type.  Upon return from this
        ///  constructor, this object is equal to \a original.

    explicit info_allocator( A const& a );
        ///< Decorates a copy of \a a.

    explicit info_allocator( A&& a );
        ///< Decorates an allocator moved from \a a.

    ~info_allocator();
        ///< Destroys this object.  Releases shared state if this allocator was
        /// the last in its copy group.

    info_allocator& operator=(info_allocator const& rhs);
        ///< Assigns to this object the value of \a rhs.  This allocator
        /// leaves its former copy group, and becomes part of the copy group of
        /// \a rhs.  If this allocator was the last member of its copy group,
        /// then the group's shared state is destroyed.  Assignment from other
        /// instantiations of \c info_allocator is supported through implicit
        /// conversion to this type, followed by invocation of this assignment
        /// operator.

    info_allocator& operator=(info_allocator&& rhs);
        ///< Assigns to this object the value of \a rhs, moving its underlying
        /// allocator.  This allocator leaves its former copy group, and
        /// becomes part of the copy group of \a rhs.  If this allocator was
        /// the last member of its copy group, then the group's shared state is
        /// destroyed.

    pointer allocate(size_type n, const_void_pointer u =nullptr);
        ///< Returns space for \a n objects of type \c T, passing \a u as a
        /// hint to the underlying allocator, or throws an exception if the
        /// space cannot be allocated.

    void deallocate(pointer p, size_type n);
        ///< Frees space for \a n objects beginning at address \p.  The
        /// behavior is undefined unless \a p was returned by a previous call
        /// to \c allocate exactly \a n objects, and has not already been
        /// deallocated.

    size_type max_size() const;
        ///< Returns the largest value that can meaningfully be passed to \c
        /// allocate.  Note that \c allocate is not guaranteed to succeed.

    A get_allocator() const;
        ///< Returns the decorated allocator.

    info_allocator select_on_container_copy_construction() const;

    size_type allocate_calls() const;
        ///< Returns the number of calls to \c allocate.

    size_type deallocate_calls() const;
        ///< Returns the number of calls to \c deallocate.

    size_type objects_all() const;
        ///< Returns the total number of objects allocated.  The result
        /// includes objectS that have been deallocated.

    size_type objects_max() const;
        ///< Returns the most simultaneous live objects seen.

    size_type objects_now() const;
        ///< Returns the number of currently live objects.

    size_type memory_all() const;
        ///< Returns the total amount of memory allocated.  The result includes
        /// memory that has been deallocated.

    size_type memory_max() const;
        ///< Returns the highest amount of live memory allocated at any time.

    size_type memory_now() const;
        ///< Returns the amount of currently live memory.
};

template <typename T, typename A>
bool operator==(
        info_allocator<T, A> const& a1
      , info_allocator<T, A> const& a2);
    ///< Returns \c true if \a a1 and \a a2 have the same value.  Objects of
    /// class \c info_allocator have the same value if their decorated
    /// allocators compare equal via \c operator==.  If allocators compare
    /// equal, storage allocated from each may be deallocated by the other.

template <typename T, typename A>
bool operator!=(
        info_allocator<T, A> const& a1
      , info_allocator<T, A> const& a2);
    ///< Returns \c true if \a a1 and \a a2 do not have the same value.
    /// Equivalent to <code>!(a1 == a2)</code>.

template <typename T, typename A, typename U>
bool operator==(
        info_allocator<
            T
          , A
        > const& a
      , info_allocator<
            U
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& b);
    ///< Returns \c true if \a a and \a b have the same value.  The allocators
    /// have the same value if \a a has the same value as the result of
    /// explicitly converting \a b to the type of \a a.  If allocators compare
    /// equal, storage allocated from each may be deallocated by the other.
    /// Note that this comparison operation is required by the C++ Standard.

template <typename T, typename A, typename U>
bool operator!=(
        info_allocator<
            T
          , A
        > const& a
      , info_allocator<
            U
          , typename std::allocator_traits<A>::template rebind_alloc<U>
        > const& b);
    ///< Returns \c true if \a a and \a b do not have the same value.
    /// Equivalent to <code>!(a == b)</code>.

}  /// \namespace unbuggy

#include "unbuggy/info_allocator.tpp"
#endif
