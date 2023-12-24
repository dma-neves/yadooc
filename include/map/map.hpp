#pragma once

#include <unordered_map>
#include <string>

#include "prism.hpp"

class map {

public:
    std::unordered_map<std::string, prism> prisms;

    void load_csv(std::string file_path);
};