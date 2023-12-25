#pragma once

#include <unordered_map>
#include <string>

#include "prism.hpp"

class map {

public:
    std::unordered_map<std::string, prism> prisms;

    void load_file(std::string file_path);
};