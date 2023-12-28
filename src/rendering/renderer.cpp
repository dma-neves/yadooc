#include "rendering/renderer.hpp"
#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
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

vertical_surface renderer::compute_surface(sf::RenderWindow* window, projection& projection_a, projection& projection_b, std::string texture_id) {

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

void renderer::render_solid(sf::RenderWindow* window, solid& _solid, std::vector<projection>& projections) {

    if(projections.size() != _solid.edges.size()) {
        // TODO
        return;
    }

    int nedges = projections.size();
    int nsurfaces = nedges;

    // Render horizontal surfaces

    sf::ConvexShape bot_surface;
    sf::ConvexShape top_surface;
    bot_surface.setPointCount(nedges);
    top_surface.setPointCount(nedges);
    for(int i = 0; i < projections.size(); i++) {

        sf::Vector2f bot_corner(projections[i].projection_plane_x, projections[i].projection_plane_y_bot);
        bot_surface.setPoint(i, bot_corner);

        sf::Vector2f top_corner(projections[i].projection_plane_x, projections[i].projection_plane_y_top);
        top_surface.setPoint(i,top_corner);
    }
    bot_surface.setFillColor(sf::Color(64, 64, 64));
    top_surface.setFillColor(sf::Color(64, 64, 64));
    // TODO: determine correct draw order (doesn't matter if they are the same color)
    window->draw(bot_surface);
    window->draw(top_surface);


    // Render walls

    float leftmost_edge_projection = std::numeric_limits<float>::max();
    float rightmost_edge_projection = std::numeric_limits<float>::min();
    int leftmost_edge_index = 0;
    int rightmost_edge_index = 0;


    for(int i = 0; i < nedges; i++) {
        if(projections[i].projection_plane_x < leftmost_edge_projection) {
            leftmost_edge_projection = projections[i].projection_plane_x;
            leftmost_edge_index = i;
        }
        if(projections[i].projection_plane_x > rightmost_edge_projection) {
            rightmost_edge_projection = projections[i].projection_plane_x;
            rightmost_edge_index = i;
        }
    }

    // Make sure we only iterate and render the surfaces that are "on front"
    int prev_index = modulo(leftmost_edge_index-1, nedges);
    int next_index = modulo(leftmost_edge_index+1, nedges);
    bool backwards_iteration = projections[prev_index].distance < projections[next_index].distance;
    int increment = backwards_iteration ? -1 : 1;

    for(int i = leftmost_edge_index; i != rightmost_edge_index; i = modulo(i+increment, nsurfaces)) {

        next_index = modulo(i+increment, nsurfaces);
        int texture_id_index = backwards_iteration ? next_index : i;
        std::string texture_id = _solid.surface_texture_ids[texture_id_index];

        vertical_surface vs = compute_surface(window, projections[i], projections[next_index], texture_id);
        render_vertical_surface(window, vs);
    }
}

std::vector<projection> renderer::compute_solid_projections(sf::RenderWindow* window, solid& _solid) {

    std::vector<projection> projections;
    for(auto& edge : _solid.edges) {

        try {
            sf::Vector2f rel_pos = edge.pos - sf::Vector2f(_camera->pos.x, _camera->pos.y);
            float distance = magnitude(rel_pos);

            sf::Vector3f top_vertex_3f(edge.pos.x, edge.pos.y, edge.top_z);
            sf::Vector2f top_projection_plane_pos = project_point(top_vertex_3f);

            sf::Vector3f bot_vertex_3f(edge.pos.x, edge.pos.y, edge.bot_z);
            sf::Vector2f bot_projection_plane_pos = project_point(bot_vertex_3f); // TODO: avoid projecting x axis twice

            float x_scale = window->getSize().x / plane_width;
            float y_scale = window->getSize().y / plane_height;

            projections.push_back({
                .distance = distance,
                .projection_plane_x = x_scale * top_projection_plane_pos.x,
                .projection_plane_y_top = y_scale * top_projection_plane_pos.y,
                .projection_plane_y_bot = y_scale * bot_projection_plane_pos.y
            });
        }
        catch(const std::exception& e) {

            // surface behind camera
            //printf("exception\n");
        }    
    }

    return projections;
}

float renderer::compute_backdrop_camera_dir_vertical_offset() {

    return tan(_camera->vertical_rot_angle) / plane_distance;
}

void renderer::render_backdrop(sf::RenderWindow* window, backdrop& _backdrop) {

    sf::Texture* sf_texture = textures.find(_backdrop.texture_id)->second.sf_texture;
    float texture_width = static_cast<float>(sf_texture->getSize().x);
    float texture_height = static_cast<float>(sf_texture->getSize().y);

    float vertical_camera_dir_offset = compute_backdrop_camera_dir_vertical_offset() ;
    float window_scale_y = window->getSize().y / plane_height;
    float vertical_window_offset = vertical_camera_dir_offset * window_scale_y;
    float texture_scale_y = texture_height / plane_height;
    float vertical_texture_offset = vertical_camera_dir_offset * texture_scale_y;

    sf::VertexArray bdrop_vertices(sf::Quads, 4);
    bdrop_vertices[0].position = sf::Vector2f(0, 0 + (vertical_window_offset < 0 ? vertical_window_offset : 0));
    bdrop_vertices[1].position = sf::Vector2f(window->getSize().x, 0 + (vertical_window_offset < 0 ? vertical_window_offset : 0));
    bdrop_vertices[2].position = sf::Vector2f(window->getSize().x, window->getSize().y / 2 + vertical_window_offset);
    bdrop_vertices[3].position = sf::Vector2f(0, window->getSize().y / 2 + vertical_window_offset);


    sf::Vector3f default_dir = DEFAULT_DIRECTION;
    sf::Vector2f default_dir_2f(default_dir.x, default_dir.y);
    sf::Vector2f camera_dir_2f(_camera->direction.x, _camera->direction.y);
    float angle = angle_between_vectors(camera_dir_2f, default_dir_2f);
    if(angle < 0)
        angle += 2*M_PI;
    angle = 2*M_PI - angle;

    float offset = angle/(2.f*M_PI) * (2.f/3.f * texture_width);
    if(offset >= texture_width * 2.f/3.f)
        offset = texture_width / 3.f;
    if(offset <= 0)
        offset = texture_width / 3.f;

    float y_slice = 0.5;

    bdrop_vertices[0].texCoords = sf::Vector2f(offset, texture_height*y_slice - vertical_texture_offset);
    bdrop_vertices[1].texCoords = sf::Vector2f(offset + texture_width / 3, texture_height*y_slice - vertical_texture_offset);
    bdrop_vertices[2].texCoords = sf::Vector2f(offset + texture_width / 3, texture_height);
    bdrop_vertices[3].texCoords = sf::Vector2f(offset, texture_height);

    // Apply the shader to the texture
    sf::RenderStates states;
    states.texture = sf_texture;

    // Draw the vertex array
    window->draw(bdrop_vertices, states);
}

void renderer::render_floor(sf::RenderWindow* window, floor_t& floor) {

    float vertical_camera_dir_offset = compute_backdrop_camera_dir_vertical_offset() ;
    float window_scale_y = window->getSize().y / plane_height;
    float vertical_window_offset = vertical_camera_dir_offset * window_scale_y;

    sf::ConvexShape floor_shape;
    floor_shape.setPointCount(4);
    floor_shape.setPoint(0, sf::Vector2f(0,window->getSize().y/2 + vertical_window_offset));    
    floor_shape.setPoint(1, sf::Vector2f(window->getSize().x,window->getSize().y/2 + vertical_window_offset));    
    floor_shape.setPoint(2, sf::Vector2f(window->getSize().x,window->getSize().y + (vertical_window_offset > 0 ? vertical_window_offset : 0)));    
    floor_shape.setPoint(3, sf::Vector2f(0,window->getSize().y + (vertical_window_offset > 0 ? vertical_window_offset : 0)));    

    sf::Texture* sf_texture = textures.find(floor.texture_id)->second.sf_texture;
    floor_shape.setTexture(sf_texture);

    window->draw(floor_shape);
}

void renderer::render(sf::RenderWindow* window, map& _map) {

    if(_map.floor.active) {

        render_floor(window, _map.floor);
    }

    if(_map._backdrop.active) {

        render_backdrop(window, _map._backdrop);
    }

    std::unordered_map<std::string, std::vector<projection>> solid_projections;
    std::map<float, std::string> solid_depths;

    for (auto it = _map.solids.begin(); it != _map.solids.end(); it++) {

        solid& _solid = it->second;
        std::vector<projection> projections = compute_solid_projections(window, _solid);
        if(projections.size() > 0) {
            float depth = compute_solid_depth(projections);

            solid_projections.insert({_solid.id, projections});
            solid_depths.insert({depth, _solid.id});
        }
    }

    for (auto it = solid_depths.rbegin(); it != solid_depths.rend(); it++) {
        std::string solid_id = it->second;
        solid& _solid = _map.solids.find(solid_id)->second;
        std::vector<projection>& projections = solid_projections.find(solid_id)->second;
        render_solid(window, _solid, projections);
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
    float theta = fabs(angle_between_vectors(point_relative_position, camera_dir_2f));
    float distance_undistorted = distance * cos(theta);

    float delta_z = point.z - _camera->pos.z;
    float delta_z_abs = fabs(delta_z);
    float projection_plane_delta_y = plane_distance / distance_undistorted * delta_z_abs;
    float projection_plane_pos_y = delta_z > 0 ?
        plane_height/2.f - projection_plane_delta_y :
        plane_height/2.f + projection_plane_delta_y;

    float camera_vertical_rotation_offset = sin(_camera->vertical_rot_angle); // artificial vertical rotation

    return sf::Vector2f(projection_plane_pos_x, projection_plane_pos_y + camera_vertical_rotation_offset);
}


float renderer::compute_solid_depth(const std::vector<projection>& projections) {

    // TODO: improve this (ex: vertical distance)
    float sum_depth = 0.0f;

    for(const projection& _projection : projections) {
        sum_depth += _projection.distance;
    }

    float average_depth = sum_depth / (float)projections.size();
    return average_depth;
}


void renderer::load_textures_file(std::string file_path) {

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