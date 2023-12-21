#pragma once

#include "rendering/renderer.hpp"

#include <chrono>

#define DT_MULTIPLIER 10.0

class yar {

public:

    yar();

    void run();
    void update(double dt);
    void render();
    void handle_events(double dt);

private:
    camera _camera;
    map _map;
    renderer _renderer;
    bool running = true;
};