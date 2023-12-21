#pragma once

#include "map/map.hpp"
#include "camera.hpp"

class renderer {

public:

    renderer(double fov = 0.785, comp_t plane_distance = 1.0f);

    void render(map _map, camera _camera);

private:
    comp_t plane_distance;
    comp_t plane_width;
};