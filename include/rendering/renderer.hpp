#pragma once

#include "map/map.hpp"
#include "camera.hpp"
#include "util/texture.hpp"
#include "util/lalgebra.hpp"
#include "rendering/vertical_surface.hpp"
#include "rendering/projection.hpp"

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

class renderer {

public:

    renderer(camera* _camera, double fov = 0.785, float plane_distance = 1.0f);
    ~renderer();

    void render(sf::RenderWindow* window, map& _map);
    void load_texture(std::string id, std::string texture_file);
    void load_textures_file(std::string file_path);

private:

    void render_solid(sf::RenderWindow* window, solid& _solid, std::vector<projection>& projections);
    void render_vertical_surface(sf::RenderWindow* window, vertical_surface& surface);
    sf::Vector2f project_point(sf::Vector3f& point);
    std::vector<projection> compute_solid_projections(sf::RenderWindow* window, solid& _solid);
    vertical_surface compute_surface(sf::RenderWindow* window, projection& projection_a, projection& projection_b, std::string texture_id);
    float compute_solid_depth(const std::vector<projection>& projections);

    camera* _camera;
    float plane_distance;
    float plane_width;
    float plane_height;

    std::unordered_map<std::string, texture> textures;
};