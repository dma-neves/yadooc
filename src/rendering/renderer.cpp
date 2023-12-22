#include "rendering/renderer.hpp"
#include <cmath>
#include <limits>

renderer::renderer(camera* _camera, double fov, float plane_distance) : _camera(_camera), plane_distance(plane_distance) {

    plane_width = 2.0 * plane_distance * tan(fov/2.0);
    plane_height = plane_width;
}

renderer::~renderer() {

    for(auto& entry : textures) {

        texture& tex = entry.second;
        delete tex.sf_texture;
    }
}

void renderer::render_vertical_surface(sf::RenderWindow* window, vertical_surface& surface) {

    sf::Texture* sf_texture = textures.find( surface.texture_id )->second.sf_texture;

    sf::VertexArray surface_shape(sf::Quads, 4);
    surface_shape[0].position = surface.top_left;
    surface_shape[1].position = surface.top_right;
    surface_shape[2].position = surface.bot_right;
    surface_shape[3].position = surface.bot_left;

    // Define texture coordinates for each vertex
    surface_shape[0].texCoords = sf::Vector2f(0, 0); // top_left
    surface_shape[1].texCoords = sf::Vector2f(sf_texture->getSize().x, 0); // top_right
    surface_shape[2].texCoords = sf::Vector2f(sf_texture->getSize().x, sf_texture->getSize().y); // bot_right
    surface_shape[3].texCoords = sf::Vector2f(0, sf_texture->getSize().y); // bot_left

    sf::RenderStates states;
    states.texture = sf_texture;

    window->draw(surface_shape, states);

}

void renderer::render_prism(sf::RenderWindow* window, prism& _prism) {

    std::vector<vertical_surface> surfaces = compute_surfaces(window, _prism);
    int nsurfaces = surfaces.size();
    float leftmost_edge = std::numeric_limits<float>::max();
    int first_surface_index = 0;


    for(int i = 0; i < surfaces.size(); i++) {

        vertical_surface& vs = surfaces[i];
        if(!vs.hidden && vs.top_left.x < leftmost_edge) {
            leftmost_edge = vs.top_left.x;
            first_surface_index = i;
        }
    }

    int prev_index = modulo(first_surface_index-1, nsurfaces);
    // Make sure we iterate first over the surfaces that are behind
    bool backwards_iteration = surfaces[prev_index].distance_right > surfaces[first_surface_index].distance_left;
    int increment = backwards_iteration ? -1 : 1;

    if(backwards_iteration)
        first_surface_index = modulo(first_surface_index-1, nsurfaces);

    // increment = iter_dir;

    
    int rendered_surfaces = 0;
    for(int i = first_surface_index; rendered_surfaces < nsurfaces; i = modulo(i+increment, nsurfaces)) {

        if(!surfaces[i].hidden) {
            render_vertical_surface(window, surfaces[i]);
            rendered_surfaces++;
        }
    }
}

std::vector<vertical_surface> renderer::compute_surfaces(sf::RenderWindow* window, prism& _prism) {

    std::vector<vertical_surface> surfaces;

    for(int i = 0; i < _prism.edges.size(); i++) {


        sf::Vector2f& edge_0 = _prism.edges[i];
        sf::Vector2f& edge_1 = _prism.edges[ modulo(i+1, _prism.edges.size()) ];

        try {

            // TODO: currently recalculating projections
            float projection_plane_x_0 = project_point(edge_0);
            float projection_plane_x_1 = project_point(edge_1);

            sf::Vector2f edge_left, edge_right;
            float projection_plane_x_left, projection_plane_x_right;
            
            if(projection_plane_x_0 < projection_plane_x_1) {
                edge_left = edge_0;
                edge_right = edge_1;
                projection_plane_x_left = projection_plane_x_0;
                projection_plane_x_right = projection_plane_x_1;
            }
            else {
                edge_left = edge_1;
                edge_right = edge_0;
                projection_plane_x_left = projection_plane_x_1;
                projection_plane_x_right = projection_plane_x_0;
            }

            sf::Vector2f camera_pos_2f = sf::Vector2f(_camera->pos.x, _camera->pos.y);
            sf::Vector2f rel_pos_left = edge_left - camera_pos_2f;
            sf::Vector2f rel_pos_right = edge_right - camera_pos_2f;

            float distance_left = magnitude(rel_pos_left);
            float distance_right = magnitude(rel_pos_right);

            float height_left = projected_height(distance_left, _prism.height );
            float height_right = projected_height(distance_right, _prism.height );
            
            float projection_plane_y_left = (plane_height-height_left)/2.f + _camera->pos.z * (height_left/2.f);
            float projection_plane_y_right = (plane_height-height_right)/2.f + _camera->pos.z * (height_right/2.f);

            float w_scale = window->getSize().x / plane_width;
            float y_scale = window->getSize().y / plane_height;

            vertical_surface vs = {
                .hidden = false,
                .distance_left = distance_left,
                .distance_right = distance_right,
                .top_left = { w_scale * projection_plane_x_left, y_scale * projection_plane_y_left },
                .top_right = { w_scale * projection_plane_x_right, y_scale * projection_plane_y_right },
                .bot_left = { w_scale * projection_plane_x_left, y_scale * (projection_plane_y_left+height_left) },
                .bot_right = { w_scale * projection_plane_x_right, y_scale * (projection_plane_y_right+height_right) },
                .texture_id = _prism.surface_texture_ids[i]
            };


            surfaces.push_back(vs);
        }
        catch(const std::exception& e) {

            // surface behind camera. no need to draw it
            vertical_surface vs = { .hidden = true };
            surfaces.push_back(vs);
            printf("exception\n");
        }
    }

    return surfaces;
}

void renderer::render(sf::RenderWindow* window, map& _map) {

    // TODO: sort prisms in render order

    for(prism& _prism : _map.prisms) {

        render_prism(window, _prism);
    }
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

    if(fabs(alpha) > M_PI/2.f)
        throw std::runtime_error("behind camera");

    float dx = plane_distance * tan(fabs(alpha));
    float projection_plane_pos_x = alpha < 0.f ? plane_width/2.f - dx : plane_width/2.f + dx;

    return projection_plane_pos_x;
}

float renderer::projected_height(float distance, float real_height) {

    return 1.f / distance * real_height;
}