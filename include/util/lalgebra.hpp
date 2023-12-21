#pragma once

#include <SFML/Graphics.hpp>

sf::Transform shear_around(sf::Vector2f origin, float dy);
float angle_between_vectors(sf::Vector2f& vec_a, sf::Vector2f& vec_b);