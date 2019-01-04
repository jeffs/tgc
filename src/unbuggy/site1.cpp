#include <iostream>
#include <string>
#include <vector>

#include "unbuggy/fml.hpp"              // for fml::string and fml::vector

int main()
{
    fml::string line;
    fml::vector<fml::string> lines;

    while (std::getline(std::cin, line))
        lines.push_back(std::move(line));

    std::sort(lines.begin(), lines.end());

    for (auto const& line : lines)
        std::cout << line << '\n';
}
