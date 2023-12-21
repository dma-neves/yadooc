#include "yar.hpp"

yar::yar() {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {

        printf("Error initializing SDL: %s\n", SDL_GetError());
        exit(1);
    }    

    sdl_window = SDL_CreateWindow("yar", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED; 
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, render_flags);
}

yar::~yar() {

    SDL_DestroyWindow(sdl_window);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_Quit();
}


void yar::run() {

    using clock_t = std::chrono::high_resolution_clock;
    using time_t = std::chrono::time_point<clock_t>;
    using duration_t = std::chrono::duration<double>;

    time_t previous_time = clock_t::now();

    _renderer.load_texture(sdl_renderer, "stone_wall", "../../../assets/sprites/Stone.png");

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

    SDL_RenderClear(sdl_renderer);

    _renderer.render(sdl_window, sdl_renderer, _map, _camera);

    SDL_RenderPresent(sdl_renderer);

    // calculates to 60 fps
    SDL_Delay(1000.0 / 60.0);
}

void yar::handle_events(double dt) {

    while (SDL_PollEvent(&sdl_event) != 0) {

        switch (sdl_event.type)
        {
        case SDL_QUIT:
            running = false;
            break;
        
        default:
            break;
        }
    }
}