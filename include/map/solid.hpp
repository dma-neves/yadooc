#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

struct edge_t {

    sf::Vector2f pos;
    float bot_z;
    float top_z;
};

struct solid {
    std::string id;
    std::vector<edge_t> edges;
    std::vector<std::string> surface_texture_ids;
};