#include "map/map.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

void map::load_file(std::string file_path) {
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return;
    }

    int num_floors;
    file >> num_floors;

    if(num_floors > 0) {
        floor.active = true;
        file >> floor.texture_id;
    }

    int num_backdrops;
    file >> num_backdrops;

    if(num_backdrops > 0) {
        _backdrop.active = true;
        file >> _backdrop.texture_id;
    }

    int num_solids;
    file >> num_solids;

    for (int i = 0; i < num_solids; ++i) {
        solid current_solid;
        file >> current_solid.id;

        int num_edges;
        file >> num_edges;

        for (int j = 0; j < num_edges; ++j) {
            edge_t edge;
            file >> edge.pos.x >> edge.pos.y >> edge.bot_z >> edge.top_z;
            current_solid.edges.push_back(edge);
        }

        for (int j = 0; j < num_edges; ++j) {
            std::string texture_id;
            file >> texture_id;
            current_solid.surface_texture_ids.push_back(texture_id);
        }

        solids.insert({current_solid.id, current_solid});
    }

    file.close();
}
