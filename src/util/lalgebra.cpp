#include "util/lalgebra.hpp"

#include <cmath>

sf::Transform shear_around(sf::Vector2f origin, float dy) {

    // sf::Transform shear(1.f, 0.f, 0.f,
    //                     dy , 1.f, 0.f,
    //                     0.f, 0.f, 1.f);

    sf::Transform shear(1.f, 0.f, 0.f,
                        dy, 1.f, 0.f,
                        0.f, 0.f, 1.f);


    sf::Transform translate_to_origin;
    translate_to_origin.translate(-origin.x, -origin.y);

    sf::Transform translate_back;
    translate_back.translate(origin.x, origin.y);

    return translate_back * shear * translate_to_origin;
}

float angle_between_vectors(sf::Vector2f& vec_a, sf::Vector2f& vec_b) {
    // Check for zero vectors
    if (vec_a.x == 0.0f && vec_a.y == 0.0f) {
        return 0.0f; // or choose an appropriate default angle
    }

    if (vec_b.x == 0.0f && vec_b.y == 0.0f) {
        return 0.0f; // or choose an appropriate default angle
    }

    float dot_product = vec_a.x * vec_b.x + vec_a.y * vec_b.y;

    // Check for zero magnitude
    if (dot_product == 0.0f) {
        return 0.0f; // vectors are perpendicular, so the angle is 90 degrees
    }

    float magnitude_a = sqrt(vec_a.x * vec_a.x + vec_a.y * vec_a.y);
    float magnitude_b = sqrt(vec_b.x * vec_b.x + vec_b.y * vec_b.y);

    float cos_theta = dot_product / (magnitude_a * magnitude_b);

    // Avoid numerical instability for very small values of cos_theta
    cos_theta = std::max(-1.0f, std::min(1.0f, cos_theta));

    float angle_rad = acos(cos_theta);

    sf::Vector3f cross_product(0.0f, 0.0f, vec_a.x * vec_b.y - vec_a.y * vec_b.x);
    float multiplier = cross_product.z > 0.f ? 1.f : -1.f;

    return angle_rad * multiplier;
}

float magnitude(sf::Vector2f vec) {

    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

sf::Vector2f rotate_vector(sf::Vector2f& vec, float angle) {

    float x = vec.x * cos(angle) - vec.y * sin(angle);
    float y = vec.x * sin(angle) + vec.y * cos(angle);

    return sf::Vector2f(x, y);
}

int modulo(int val, int max) {

    if(val < 0) {

        return (val + max) % max;
    }
    else {

        return val % max;
    }
}