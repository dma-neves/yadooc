#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

struct prism {
    std::string id;
    float height;
    float pos_z;
    std::vector<sf::Vector2f> edges;
    std::vector<std::string> surface_texture_ids;
};