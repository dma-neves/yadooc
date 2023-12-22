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

vertical_surface renderer::compute_surface(sf::RenderWindow* window, projection& projection_a, projection& projection_b, float height, std::string texture_id) {    

        bool a_left_of_b = projection_a.projection_plane_x < projection_b.projection_plane_x;
        projection& projection_left_edge = a_left_of_b ? projection_a : projection_b;
        projection& projection_right_edge = a_left_of_b ? projection_b : projection_a;

        float projection_plane_x_left = projection_left_edge.projection_plane_x; 
        float projection_plane_x_right = projection_right_edge.projection_plane_x;
        float distance_left = projection_left_edge.distance; 
        float distance_right = projection_right_edge.distance;

        float height_left = projected_height(distance_left, height );
        float height_right = projected_height(distance_right, height );

        float camera_height_offset_left =  _camera->pos.z * (height_left/2.f);
        float camera_height_offset_right =  _camera->pos.z * (height_right/2.f);
        float camera_vertical_rotation_offset = sin(_camera->vertical_rot_angle);

        float projection_plane_y_left = (plane_height-height_left)/2.f + camera_height_offset_left + camera_vertical_rotation_offset;
        float projection_plane_y_right = (plane_height-height_right)/2.f + camera_height_offset_right + camera_vertical_rotation_offset;

        float w_scale = window->getSize().x / plane_width;
        float y_scale = window->getSize().y / plane_height;

        vertical_surface vs = {
            .top_left = { w_scale * projection_plane_x_left, y_scale * projection_plane_y_left },
            .top_right = { w_scale * projection_plane_x_right, y_scale * projection_plane_y_right },
            .bot_left = { w_scale * projection_plane_x_left, y_scale * (projection_plane_y_left+height_left) },
            .bot_right = { w_scale * projection_plane_x_right, y_scale * (projection_plane_y_right+height_right) },
            .texture_id = texture_id
        };

        return vs;
}

void renderer::render_prism(sf::RenderWindow* window, prism& _prism) {

    std::vector<projection> projections = compute_prism_projections(_prism);

    if(projections.size() != _prism.edges.size()) {

        // TODO
        return;
    }


    int nedges = projections.size();
    int nsurfaces = nedges;

    float leftmost_edge_projection = std::numeric_limits<float>::max();
    int leftmost_edge_index = 0;

    for(int i = 0; i < nedges; i++) {

        if(projections[i].projection_plane_x < leftmost_edge_projection) {
            leftmost_edge_projection = projections[i].projection_plane_x;
            leftmost_edge_index = i;
        }
    }

    // Make sure we iterate first over the surfaces that are behind
    int prev_index = modulo(leftmost_edge_index-1, nedges);
    bool backwards_iteration = projections[prev_index].distance > projections[leftmost_edge_index].distance;
    int increment = backwards_iteration ? -1 : 1;

    std::vector<vertical_surface> vertical_surfaces;
    for(int i = leftmost_edge_index; vertical_surfaces.size() < nsurfaces; i = modulo(i+increment, nsurfaces)) {

        int next_index = modulo(i+increment, nsurfaces);
        int texture_id_index = backwards_iteration ? next_index : i;
        std::string texture_id = _prism.surface_texture_ids[texture_id_index];

        vertical_surface vs = compute_surface(window, projections[i], projections[next_index], _prism.height, texture_id);
        render_vertical_surface(window, vs);
        vertical_surfaces.push_back(vs);
    }
    
    if(_camera->pos.z != 0) {

        // Render horizontal surface

        sf::ConvexShape horizontal_surface;
        horizontal_surface.setPointCount(nedges);
        for(int i = 0; i < vertical_surfaces.size(); i++) {

            if(_camera->pos.z > 0) {
                horizontal_surface.setPoint(i, vertical_surfaces[i].top_left);
            }
            else if (_camera->pos.z < 0){
                horizontal_surface.setPoint(i, vertical_surfaces[i].bot_left);
            }
        }

        printf("nedges: %d\n", nedges);

        horizontal_surface.setFillColor(sf::Color(128, 128, 128));
        window->draw(horizontal_surface);
    }
}

std::vector<projection> renderer::compute_prism_projections(prism& _prism) {


    std::vector<projection> projections;
    for(auto& edge : _prism.edges) {

        try {
            float projection_plane_x = project_point(edge);
            sf::Vector2f camera_pos_2f = sf::Vector2f(_camera->pos.x, _camera->pos.y);
            sf::Vector2f rel_pos = edge - camera_pos_2f;
            float distance = magnitude(rel_pos);

            projections.push_back({
                .distance = distance,
                .projection_plane_x = projection_plane_x
            });
        }
        catch(const std::exception& e) {

            // surface behind camera
            printf("exception\n");
        }    
    }

    return projections;
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
        printf("Failed to load image %s\n", texture_file.c_str());
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