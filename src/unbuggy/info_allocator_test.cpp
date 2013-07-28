#include "unbuggy/info_allocator.hpp"

struct item {
};

int main()
{
    unbuggy::info_allocator<item> a;
    (void)a;
}
