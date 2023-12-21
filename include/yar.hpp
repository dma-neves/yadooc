#pragma once


#include <chrono>
#include <SDL2/SDL.h>

#include "rendering/renderer.hpp"

#define DT_MULTIPLIER 10.0

class yar {

public:

    yar();
    ~yar();

    void run();
    void update(double dt);
    void render();
    void handle_events(double dt);

private:
    camera _camera;
    map _map;
    renderer _renderer;
    bool running = true;

    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
    SDL_Event sdl_event;
};