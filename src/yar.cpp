#include "yar.hpp"

yar::yar() : window(sf::VideoMode(800, 800), "yar"), _camera(),  _renderer(&_camera) {

}

yar::~yar() {


}


void yar::run() {

    using clock_t = std::chrono::high_resolution_clock;
    using time_t = std::chrono::time_point<clock_t>;
    using duration_t = std::chrono::duration<double>;

    time_t previous_time = clock_t::now();

    _renderer.load_texture("stone_wall", "../../../assets/sprites/Stone.png");

    while(running) {

        time_t current_time = clock_t::now();
        double dt = ( static_cast<duration_t>(current_time - previous_time) ).count() * DT_MULTIPLIER;
        previous_time = current_time;

        handle_events(dt);
        update(dt);
        render();
    }
}

void yar::update(double dt) {

}

void yar::render() {

    window.clear();
    
    _renderer.render(&window, _map);

    window.display();
}

void yar::handle_events(double dt) {

    while (window.pollEvent(event)) {

        if (event.type == sf::Event::Closed)
            window.close();

        switch (event.type)
        {
        case sf::Event::Closed:
            window.close();
            running = false;
            break;

        case sf::Event::KeyPressed:
            if(event.key.code == sf::Keyboard::Escape) {

                window.close();
                running = false;
            }
        
        default:
            break;
        }
    }
}