/// \file scoped.hpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#ifndef UNBUGGY_INCLUDED_SCOPED
#define UNBUGGY_INCLUDED_SCOPED

#include "unbuggy/counting_allocator.hpp"

#include <scoped_allocator>     // scoped_allocator_adaptor

namespace unbuggy {

template <typename T, typename A =std::allocator<T> >
using scoped_counting_allocator =
    std::scoped_allocator_adaptor<counting_allocator<T, A> >;

}   /// \namespace unbuggy

#endif
