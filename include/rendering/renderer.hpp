#pragma once

#include "map/map.hpp"
#include "camera.hpp"
#include "util/texture.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

class renderer {

public:

    renderer(double fov = 0.785, comp_t plane_distance = 1.0f);
    ~renderer();

    void render(SDL_Window* sdl_window, SDL_Renderer* sdl_renderer, map _map, camera _camera);
    void load_texture(SDL_Renderer* sdl_renderer, std::string id, std::string texture_file);

private:

    void render_prism(SDL_Window* sdl_window, SDL_Renderer* sdl_renderer, prism& _prism);

    comp_t plane_distance;
    comp_t plane_width;

    std::unordered_map<std::string, texture> textures;
};