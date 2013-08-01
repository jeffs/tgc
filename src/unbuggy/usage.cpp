
/// Example 1: Determining the memory usage of an object
/// ----------------------------------------------------
/// It is often desirable to determine how much memory is allocated by an
/// object during its lifetime.  Suppose we have a program that sorts lines of
/// input in a vector, and we wish to know how much memory is required
/// collectively by the vector and all the strings.

#include "unbuggy/info_allocator.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::sort;
using std::string;
using std::vector;

using unbuggy::info_allocator;

int main()
{
    // Create a container to hold the lines.

    vector<string, info_allocator<string> > lines;

    // Read all lines into the container.

    for (string line; getline(cin, line);)
        lines.push_back(line);

    // Sort the container.

    sort(lines.begin(), lines.end());

    // Print each line.

    for (auto const& line: lines)
        cout << line << '\n';

    std::clog << lines.get_allocator().objects_now() << '\n';
}
