#include "rendering/renderer.hpp"
#include <cmath>

renderer::renderer(camera* _camera, double fov, float plane_distance) : _camera(_camera), plane_distance(plane_distance) {

    plane_width = 2.0 * plane_distance * tan(fov/2.0);
}

renderer::~renderer() {

    for(auto& entry : textures) {

        texture& tex = entry.second;
        delete tex.texture;
    }
}

void renderer::render_vertical_surface(sf::RenderWindow* window, vertical_surface& surface) {

    // In order to get the texture to shear, we first define the
    // convex shape as a non sheared rectangle based on the surface's coordinates
    // and then draw the shape with the appropriate shear transform

    sf::ConvexShape surface_shape;
    surface_shape.setPoint(0, surface.top_left);
    surface_shape.setPoint(1, sf::Vector2f(surface.top_right.x, surface.top_left.y));
    surface_shape.setPoint(3, sf::Vector2f(surface.bot_right.x, surface.bot_left.y));
    surface_shape.setPoint(2, surface.bot_left);

    float dy = surface.top_right.y - surface.top_left.y;
    auto shear_transform = shear_around(surface.top_left, dy);

    window->draw(surface_shape, shear_transform);
}

void renderer::render_prism(sf::RenderWindow* window, prism& _prism) {

    for(sf::Vector2f& edge : _prism.edges) {

        float projection_plane_x_pos = project_point(edge);
    }
}

void renderer::render(sf::RenderWindow* window, map& _map) {

    // TODO: sort prisms in render order

    for(prism& _prism : _map.prisms) {

        
    }

    // TEST

    // texture& tex = textures.find("stone_wall")->second;

    // // Define the polygon vertices
    // sf::ConvexShape polygon;
    // polygon.setPointCount(4); // Change this to the number of vertices in your polygon

    // // Set the position of each vertex
    // polygon.setPoint(0, sf::Vector2f(100, 100));
    // polygon.setPoint(1, sf::Vector2f(200, 100));
    // polygon.setPoint(2, sf::Vector2f(200, 200));
    // polygon.setPoint(3, sf::Vector2f(100, 200));

    // polygon.setTexture(tex.texture);

    // window->draw(polygon, shear_around(polygon.getPoint(0), 0.2f));



    // polygon.setPoint(0, sf::Vector2f(300, 100));
    // polygon.setPoint(1, sf::Vector2f(400, 100));
    // polygon.setPoint(2, sf::Vector2f(400, 200));
    // polygon.setPoint(3, sf::Vector2f(300, 200));


    // polygon.setTexture(tex.texture);

    // window->draw(polygon);
}

void renderer::load_texture(std::string id, std::string texture_file) {

    sf::Texture* tex = new sf::Texture;
    if (!tex->loadFromFile(texture_file)) {
        printf("Failed to load image %s\n", texture_file);
    }
    else {

        textures.insert({id, {id,tex}});
    }
}


float renderer::project_point(sf::Vector2f& point) {

    sf::Vector2f camera_dir = sf::Vector2f(_camera->direction.x, _camera->direction.y);
    sf::Vector2f camera_pos = sf::Vector2f(_camera->pos.x, _camera->pos.y);
    sf::Vector2f point_relative_position = point - camera_pos;

    float alpha = angle_between_vectors(camera_dir, point_relative_position);
    float dx = plane_distance * tan(fabs(alpha));
    float projection_plane_pos_x = alpha > 0.f ? plane_width/2.f - dx : plane_width/2.f + dx;

    return projection_plane_pos_x;
}