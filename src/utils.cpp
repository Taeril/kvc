#include "utils.hpp"

#include <fstream>
#include <iterator>

std::string read_file(std::string const& path) {
    std::ifstream file(path, std::ios::binary);
    return std::string(std::istreambuf_iterator{file}, {});
}

