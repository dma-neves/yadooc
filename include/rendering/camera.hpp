#pragma once

#include "util/vec3.hpp"

#define MAX_VERTICAL_TILT 0.785

class camera {

public:
    static const vec3 default_direction;
    static const vec3 default_position;

    vec3 pos;
    vec3 direction;

    camera(vec3 pos = default_position, vec3 direction = default_direction);
};