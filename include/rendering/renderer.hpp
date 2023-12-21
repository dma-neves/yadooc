#pragma once

#include "map/map.hpp"
#include "camera.hpp"
#include "util/texture.hpp"
#include "util/lalgebra.hpp"
#include "util/vertical_surface.hpp"

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

class renderer {

public:

    renderer(camera* _camera, double fov = 0.785, float plane_distance = 1.0f);
    ~renderer();

    void render(sf::RenderWindow* window, map& _map);
    void load_texture(std::string id, std::string texture_file);

private:

    void render_prism(sf::RenderWindow* window, prism& _prism);
    void render_vertical_surface(sf::RenderWindow* window, vertical_surface& surface);
    float project_point(sf::Vector2f& point);

    camera* _camera;
    float plane_distance;
    float plane_width;

    std::unordered_map<std::string, texture> textures;
};