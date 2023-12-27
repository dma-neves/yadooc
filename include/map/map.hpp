#pragma once

#include <unordered_map>
#include <string>

#include "solid.hpp"

class map {

public:
    std::unordered_map<std::string, solid> solids;

    void load_file(std::string file_path);
};