#pragma once

#include <SDL2/SDL.h>
#include <string>

struct texture {

    std::string id;
    SDL_Texture* texture;
    SDL_Rect* rect;
};