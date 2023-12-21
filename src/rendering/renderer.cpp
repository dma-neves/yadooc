#include "rendering/renderer.hpp"
#include <cmath>

renderer::renderer(double fov = 0.785, comp_t plane_distance = 1.0f) : plane_distance(plane_distance) {

    plane_width = 2.0 * plane_distance * tan(fov/2.0);
}


void renderer::render(map _map, camera _camera) {


}
