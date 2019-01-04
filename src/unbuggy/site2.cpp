#include <iostream>
#include <string>
#include <vector>

#include "unbuggy/fml.hpp"

using namespace std;

int main()
{
    fml::vector<fml::string> lines;                         // std => fml

    for (fml::string line; std::getline(std::cin, line);)   // std => fml
        lines.push_back(std::move(line));

    std::sort(lines.begin(), lines.end());

    for (auto const& line : lines)
        std::cout << line << '\n';
}
