#ifndef INCLUDED_UNBUGGY_INFO_ALLOCATOR
#define INCLUDED_UNBUGGY_INFO_ALLOCATOR

#include <iostream> // XXX
#include <memory>

namespace unbuggy {

/**
 * T - the allocated type
 * A - the wrapped allocator type
 */
template <typename T, typename A =std::allocator<T> >
class info_allocator {

    A m_a;  // the wrapped allocator

  public:
    typedef T value_type;

    /** Wrap a default-constructed instance of `A`. */
    info_allocator();

    ///** Copy `o`. */
    //info_allocator(info_allocator const& o);

    /** Wrap a copy of `a`. */
    info_allocator(A const& a);
};

template <typename T, typename A>
info_allocator<T, A>::info_allocator()
  : m_a( )
{
}

//template <typename T, typename A>
//info_allocator<T, A>::info_allocator(info_allocator const& o)

template <typename T, typename A>
info_allocator<T, A>::info_allocator(A const& a)
  : m_a( a )
{
    std::cout << __PRETTY_FUNCTION__ << '\n';
}

} // close namespace unbuggy

namespace unb = unbuggy;

#endif
