#include "yar.hpp"

yar::yar() {

}

void yar::run() {

    using clock_t = std::chrono::high_resolution_clock;
    using time_t = std::chrono::time_point<clock_t>;
    using duration_t = std::chrono::duration<double>;

    time_t previous_time = clock_t::now();

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

    _renderer.render(_map, _camera);
}

void yar::handle_events(double dt) {

}