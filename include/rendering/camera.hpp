#pragma once

#include "util/vec3.hpp"

#define MAX_VERTICAL_TILT 0.785

#define DEFAULT_POSITION {1.f, 0.f, 0.f}
#define DEFAULT_DIRECTION {0.f, 0.f, 0.f}

class camera {

public:
    static const vec3 default_direction;
    static const vec3 default_position;

    vec3 pos;
    vec3 direction;

    camera(vec3 pos = DEFAULT_POSITION, vec3 direction = DEFAULT_DIRECTION);
};