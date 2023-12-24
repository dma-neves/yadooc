#include "map/map.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

void map::load_csv(std::string file_path) {
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return;
    }

    int num_prisms;
    file >> num_prisms;

    for (int i = 0; i < num_prisms; ++i) {
        prism current_prism;
        file >> current_prism.id >> current_prism.height >> current_prism.pos_z;

        int num_edges;
        file >> num_edges;

        for (int j = 0; j < num_edges; ++j) {
            sf::Vector2f edge;
            file >> edge.x >> edge.y;
            current_prism.edges.push_back(edge);
        }

        for (int j = 0; j < num_edges; ++j) {
            std::string texture_id;
            file >> texture_id;
            current_prism.surface_texture_ids.push_back(texture_id);
        }

        prisms.insert({current_prism.id, current_prism});
    }

    file.close();
}
