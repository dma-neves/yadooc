#include "rendering/renderer.hpp"
#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>

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

        vertical_surface vs = {
            .top_left = { projection_left_edge.projection_plane_x, projection_left_edge.projection_plane_y_top },
            .top_right = { projection_right_edge.projection_plane_x, projection_right_edge.projection_plane_y_top },
            .bot_left = { projection_left_edge.projection_plane_x, projection_left_edge.projection_plane_y_bot },
            .bot_right = { projection_right_edge.projection_plane_x, projection_right_edge.projection_plane_y_bot },
            .texture_id = texture_id
        };

        return vs;
}

void renderer::render_prism(sf::RenderWindow* window, prism& _prism, std::vector<projection>& projections) {


    //std::vector<projection> projections = compute_prism_projections(window, _prism);

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

    int rendered_surfaces = 0;
    for(int i = leftmost_edge_index; rendered_surfaces < nsurfaces; i = modulo(i+increment, nsurfaces)) {

        int next_index = modulo(i+increment, nsurfaces);
        int texture_id_index = backwards_iteration ? next_index : i;
        std::string texture_id = _prism.surface_texture_ids[texture_id_index];

        vertical_surface vs = compute_surface(window, projections[i], projections[next_index], _prism.height, texture_id);
        render_vertical_surface(window, vs);
        rendered_surfaces++;
    }
    
    // Render horizontal surface
    float top = _prism.pos_z + _prism.height/2.f;
    float bot = _prism.pos_z - _prism.height/2.f;
    if(_camera->pos.z > top || _camera->pos.z < bot) { // TODO: camera at level of top edge

        sf::ConvexShape horizontal_surface;
        horizontal_surface.setPointCount(nedges);
        for(int i = 0; i < projections.size(); i++) {

            if(_camera->pos.z > top) {
                sf::Vector2f top_corner(projections[i].projection_plane_x, projections[i].projection_plane_y_top);
                horizontal_surface.setPoint(i,top_corner);
            }
            else if (_camera->pos.z < bot){
                sf::Vector2f bot_corner(projections[i].projection_plane_x, projections[i].projection_plane_y_bot);
                horizontal_surface.setPoint(i, bot_corner);
            }
        }

        horizontal_surface.setFillColor(sf::Color(64, 64, 64));
        // horizontal_surface.setTexture(textures.find("stone_wall")->second.sf_texture);
        window->draw(horizontal_surface);
    }
}

std::vector<projection> renderer::compute_prism_projections(sf::RenderWindow* window, prism& _prism) {

    std::vector<projection> projections;
    for(auto& edge : _prism.edges) {

        try {
            sf::Vector2f rel_pos = edge - sf::Vector2f(_camera->pos.x, _camera->pos.y);
            sf::Vector2f camera_dir_2f(_camera->direction.x, _camera->direction.y);
            float distance = magnitude(rel_pos);
            float theta = fabs(angle_between_vectors(rel_pos, camera_dir_2f));
            float distance_undistorted = distance * cos(theta);
            // float height = projected_height(distance_undistorted, _prism.height);
            float height = projected_height(distance, _prism.height);

            sf::Vector3f top_vertex_3f(edge.x, edge.y, _prism.pos_z + _prism.height/2.f);
            sf::Vector2f projection_plane_pos = project_point(top_vertex_3f);

            float projection_plane_y_top = projection_plane_pos.y;
            float projection_plane_y_bot = projection_plane_y_top + height;

            float x_scale = window->getSize().x / plane_width;
            float y_scale = window->getSize().y / plane_height;

            projections.push_back({
                .distance = distance,
                .projection_plane_x = x_scale * projection_plane_pos.x,
                .projection_plane_y_top = y_scale * projection_plane_y_top,
                .projection_plane_y_bot = y_scale * projection_plane_y_bot
            });
        }
        catch(const std::exception& e) {

            // surface behind camera
            //printf("exception\n");
        }    
    }

    return projections;
}

void renderer::render(sf::RenderWindow* window, map& _map) {

    // TODO: sort prisms in render order

    std::unordered_map<std::string, std::vector<projection>> prism_projections;
    std::map<float, std::string> prism_depths;

    for (auto it = _map.prisms.begin(); it != _map.prisms.end(); it++) {

        prism& _prism = it->second;
        std::vector<projection> projections = compute_prism_projections(window, _prism);
        if(projections.size() > 0) {
            float depth = compute_prism_depth(projections);

            prism_projections.insert({_prism.id, projections});
            prism_depths.insert({depth, _prism.id});
        }
    }

    for (auto it = prism_depths.rbegin(); it != prism_depths.rend(); it++) {
        std::string prism_id = it->second;
        prism& _prism = _map.prisms.find(prism_id)->second;
        std::vector<projection>& projections = prism_projections.find(prism_id)->second;
        render_prism(window, _prism, projections);
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

sf::Vector2f renderer::project_point(sf::Vector3f& point) {

    // X axis projection
    
    sf::Vector2f point_2f(point.x, point.y);
    sf::Vector2f camera_dir_2f = sf::Vector2f(_camera->direction.x, _camera->direction.y);
    sf::Vector2f camera_pos_2f = sf::Vector2f(_camera->pos.x, _camera->pos.y);
    sf::Vector2f point_relative_position = point_2f - camera_pos_2f;

    float alpha = angle_between_vectors(camera_dir_2f, point_relative_position);

    if(fabs(alpha) > M_PI/2.f)
        throw std::runtime_error("behind camera");

    float dx = plane_distance * tan(fabs(alpha));
    float projection_plane_pos_x = alpha < 0.f ? 
        plane_width/2.f - dx : 
        plane_width/2.f + dx;

    // Y axis projection

    float distance = magnitude(point_relative_position);
    float delta_z = point.z - _camera->pos.z;
    float delta_z_abs = fabs(delta_z);
    float projection_plane_delta_y = plane_distance / distance * delta_z_abs;
    float projection_plane_pos_y = delta_z > 0 ?
        plane_height/2.f - projection_plane_delta_y :
        plane_height/2.f + projection_plane_delta_y;

    float camera_vertical_rotation_offset = sin(_camera->vertical_rot_angle); // artificial vertical rotation

    return sf::Vector2f(projection_plane_pos_x, projection_plane_pos_y + camera_vertical_rotation_offset);
}

float renderer::projected_height(float distance, float real_height) {

    return 1.f / distance * real_height;
}

float renderer::compute_prism_depth(const std::vector<projection>& projections) {

    float sum_depth = 0.0f;

    for(const projection& _projection : projections) {
        sum_depth += _projection.distance;
    }

    float average_depth = sum_depth / (float)projections.size();
    return average_depth;
}


void renderer::load_textures_csv(std::string file_path) {

    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return;
    }
    
    int num_textures;
    file >> num_textures;
    for(int i = 0; i < num_textures; i++) {

        std::string texture_id, texture_file;
        file >> texture_id >> texture_file;
        load_texture(texture_id, texture_file);
    }
}