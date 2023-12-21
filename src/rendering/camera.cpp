#include "camera.hpp"

static const vec3 default_direction = (vec3){1.f, 0.f, 0.f};
static const vec3 default_position = (vec3){0.f, 0.f, 0.f};

camera::camera(vec3 pos, vec3 direction) : pos(pos), direction(direction) {
        
}