#pragma once

#include <SFML/Graphics.hpp>
#include "util/texture.hpp"

struct vertical_surface {

    bool hidden;
    float distance_left, distance_right;
    sf::Vector2f top_left, top_right, bot_left, bot_right;
    std::string texture_id;
};