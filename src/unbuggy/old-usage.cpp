#include "unbuggy/fml.hpp"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <scoped_allocator>
#include <string>
#include <vector> 

void log(char const* text, std::size_t size)
{
    std::cout << std::left << std::setw(40) << text;
    std::cout << std::right << std::setw(8) << size;
    std::cout << '\n';
}

int main()
{
    // Counting memory
    // ---------------
    // Counting the total memory consumed by a simple container is easy.
    // Suppose that we have some code like the following, and we wish to know
    // how much memory is consumed by the vector:

    {
        std::vector<std::string> v;

        for (int i = 0; i < 1000; ++i)
            v.emplace_back('x', i);
    }

    // To maintain simple statistics of memory allocation, we use a _counting_
    // allocator, rather than the default allocator:

    {
        std::vector<std::string, fml::counting_allocator<std::string> > v;

        for (int i = 0; i < 1000; ++i)
            v.emplace_back('x', i);

        auto a = v.get_allocator();

        std::cout
            << "Memory used by vector: " << a.memory() << '\n'
            << "Allocations by vector: " << a.allocations() << '\n';
    }

    // The statistics from the counting allocator consider only the vector
    // itself, not the contained elements.  To determine the total memory
    // consumed by the vector and its contents, the element (i.e., string) type
    // must use a counting allocator, and the container allocator must be
    // _scoped_:

    {
        typedef std::basic_string<
                    char
                  , std::char_traits<char>
                  , fml::counting_allocator<char> >
                string_t;

        std::vector<string_t, fml::scoped_counting_allocator<string_t> > v;

        for (int i = 0; i < 1000; ++i)
            v.emplace_back(i, 'x');

        auto a = v.get_allocator();

        std::cout
            << "Memory used by vector and strings: " << a.memory() << '\n'
            << "Allocations by vector and strings: " << a.allocations()
            << '\n';
    }

    // Using custom allocators is so common that FML provides a alias templates
    // for exactly this purpose.  The default allocator type provided by FML is
    // `scoped_counting_allocator`:

    {
        fml::vector<fml::string> v;

        for (int i = 0; i < 1000; ++i)
            v.emplace_back(i, 'x');
    }

    // FML allocators work correctly with const containers, including
    // containers directly initialized by initializer lists:

    {
        fml::vector<fml::string> const v{ "hello", "world" };
    }
}
