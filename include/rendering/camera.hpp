#pragma once

#include <SFML/Graphics.hpp>

#define MAX_VERTICAL_TILT 0.785

#define DEFAULT_POSITION {0.f, 0.f, 0.f}
#define DEFAULT_DIRECTION {1.f, 0.f, 0.f}

class camera {

public:

    sf::Vector3f pos;
    sf::Vector3f direction;
    float vertical_rot_angle = 0; // this information should be in the direction vector

    camera(sf::Vector3f pos = DEFAULT_POSITION, sf::Vector3f direction = DEFAULT_DIRECTION);
};