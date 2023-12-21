#pragma once

#include <vector>
#include <string>

#include "util/vec2.hpp"

struct prism {

    std::vector<vec2> edges;
    std::vector<std::string> surface_texture_ids;
    comp_t height;
};