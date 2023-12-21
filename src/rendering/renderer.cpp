#include "rendering/renderer.hpp"
#include <cmath>

renderer::renderer(double fov, comp_t plane_distance) : plane_distance(plane_distance) {

    plane_width = 2.0 * plane_distance * tan(fov/2.0);
}

renderer::~renderer() {

    for(auto& entry : textures) {

        texture& tex = entry.second;
        SDL_DestroyTexture(tex.texture);
        delete tex.rect;
    }
}

void renderer::render_prism(SDL_Window* sdl_window, SDL_Renderer* sdl_renderer, prism& _prism) {



    //SDL_RenderCopy(rend, tex, NULL, &dest);
}

void renderer::render(SDL_Window* sdl_window, SDL_Renderer* sdl_renderer, map _map, camera _camera) {

        //SDL_RenderCopy(rend, tex, NULL, &dest);

    // TODO: sort prisms in render order

    for(prism& _prism : _map.prisms) {

        
    }

    // TEST

    texture& tex = textures.find("stone_wall")->second;
    // tex.rect->x = 200;
    // tex.rect->y = 200;
    // tex.rect->w = 150;
    // tex.rect->h = 100;

    // SDL_RenderCopy(sdl_renderer, tex.texture, NULL, tex.rect);


    SDL_Rect srcRect = *tex.rect; // { 0, 0, 128, 128 }; // Source rectangle (entire texture)
    SDL_FRect destRect = { 300, 300, 100, 100 }; // Destination rectangle (parallelogram)


    //     // Define the destination parallelogram (sheared rectangle)
    // SDL_FRect destRect = { x1, y1, parallelogramWidth, parallelogramHeight };

    // Apply shearing transformation
    SDL_FPoint center = { destRect.x + destRect.w / 2, destRect.y + destRect.h / 2 };
    SDL_RenderCopyExF(sdl_renderer, tex.texture, &srcRect, &destRect, 0, NULL, SDL_FLIP_NONE);

    // SDL_RenderCopyEx(sdl_renderer, tex.texture, &srcRect, &destRectInt, 0.0, NULL, SDL_FLIP_NONE);
}

void renderer::load_texture(SDL_Renderer* sdl_renderer, std::string id, std::string texture_file) {

    SDL_Surface* surface = IMG_Load(texture_file.c_str());
    if (!surface) {
        
        printf("Failed to load image: %s\n", IMG_GetError());
    }
    else {

        SDL_Texture* tex = SDL_CreateTextureFromSurface(sdl_renderer, surface);
        SDL_Rect* rect = new SDL_Rect();
        SDL_QueryTexture(tex, NULL, NULL, &rect->w, &rect->h);

        textures.insert( { id, {id, tex, rect} });
        SDL_FreeSurface(surface);
    }
}
