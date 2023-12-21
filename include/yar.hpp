#pragma once


#include <chrono>
#include <SFML/Graphics.hpp>

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

    sf::RenderWindow window;
    sf::Event event;

    camera _camera;
    map _map;
    renderer _renderer;
    bool running = true;
};