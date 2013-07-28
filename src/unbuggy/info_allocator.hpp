#ifndef INCLUDED_UNBUGGY_INFO_ALLOCATOR
#define INCLUDED_UNBUGGY_INFO_ALLOCATOR

namespace unbuggy {

template <typename T>
class info_allocator {
  public:
    typedef T value_type;
};

} // close namespace unbuggy

namespace unb = unbuggy;

#endif
