/// \file fml.hpp
///
/// \copyright Copyright 2013 Unbuggy Software LLC.  All rights reserved.

#include "unbuggy/scoped.hpp"

#include <string>
#include <vector>

namespace unbuggy {

typedef std::basic_string<
        char
      , std::char_traits<char>
      , counting_allocator<char> >
  string;

template <typename T, typename A =scoped_counting_allocator<T> >
using vector = std::vector<T, A>;

}   /// \namespace unbuggy

namespace fml = unbuggy;

