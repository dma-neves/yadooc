#pragma once


#include <chrono>
#include <SFML/Graphics.hpp>
#include <cmath>

#include "rendering/renderer.hpp"

#define DT_MULTIPLIER 1.0
#define FPS 60.0

#define CAMERA_SPEED 3.f
#define CAMERA_ROT_SPEED 2.f

class yadooc {

public:

    yadooc();
    ~yadooc();

    void run();
    void update(double dt);
    void render();
    void handle_events(double dt);
    void handle_keys(double dt);

private:

    void load_map();

    sf::RenderWindow window;
    sf::Event event;

    camera _camera;
    map _map;
    renderer _renderer;
    bool running = true;
    std::unordered_map<sf::Keyboard::Key, bool> keys_pressed;
    double fps_timer = 0.0;
};