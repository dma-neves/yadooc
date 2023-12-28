#pragma once

#include <unordered_map>
#include <string>

#include "solid.hpp"
#include "backdrop.hpp"
#include "floor.hpp"

class map {

public:
    floor_t floor;
    backdrop _backdrop;
    std::unordered_map<std::string, solid> solids;

    void load_file(std::string file_path);
};