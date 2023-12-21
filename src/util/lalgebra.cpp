#include "util/lalgebra.hpp"

#include <cmath>

sf::Transform shear_around(sf::Vector2f origin, float dy) {

    sf::Transform shear(1.f, 0.f, 0.f,
                        dy , 1.f, 0.f,
                        0.f, 0.f, 1.f);


    sf::Transform translate_to_origin;
    translate_to_origin.translate(-origin.x, -origin.y);

    sf::Transform translate_back;
    translate_back.translate(origin.x, origin.y);

    return translate_back * shear * translate_to_origin;
}

float angle_between_vectors(const sf::Vector2f& vec_a, const sf::Vector2f& vec_b) {

    float dot_product = vec_a.x * vec_b.x + vec_a.y * vec_b.y;

    float magnitude_a = sqrt(vec_a.x * vec_a.x + vec_a.y * vec_a.y);
    float magnitude_b = sqrt(vec_b.x * vec_b.x + vec_b.y * vec_b.y);

    float cos_theta = dot_product / (magnitude_a * magnitude_b);

    float angle_rad = acos(cos_theta);

    sf::Vector3f cross_product(0.0f, 0.0f, vec_a.x * vec_b.y - vec_a.y * vec_b.x);
    float multiplier = cross_product.z > 0.f ? 1.f : -1.f;

    return angle_rad * multiplier;
}