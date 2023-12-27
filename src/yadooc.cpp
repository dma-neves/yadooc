#include "yadooc.hpp"

#include <cmath>

yadooc::yadooc() : window(sf::VideoMode(800, 800), "yadooc"), _camera(),  _renderer(&_camera) {

    keys_pressed.insert({sf::Keyboard::Escape, false});
    keys_pressed.insert({sf::Keyboard::W, false});
    keys_pressed.insert({sf::Keyboard::A, false});
    keys_pressed.insert({sf::Keyboard::S, false});
    keys_pressed.insert({sf::Keyboard::D, false});
    keys_pressed.insert({sf::Keyboard::Space, false});
    keys_pressed.insert({sf::Keyboard::LControl, false});
    keys_pressed.insert({sf::Keyboard::Left, false});
    keys_pressed.insert({sf::Keyboard::Right, false});
    keys_pressed.insert({sf::Keyboard::Up, false});
    keys_pressed.insert({sf::Keyboard::Down, false});

    _renderer.load_textures_file("../../../assets/maps/doom_stairs_textures.txt");
    _map.load_file("../../../assets/maps/doom_stairs.txt");

    // _renderer.load_textures_file("../../../assets/maps/sd_textures.txt");
    // _map.load_file("../../../assets/maps/sd.txt");
}

yadooc::~yadooc() {


}

void yadooc::run() {

    using clock_t = std::chrono::high_resolution_clock;
    using time_t = std::chrono::time_point<clock_t>;
    using duration_t = std::chrono::duration<double>;

    time_t previous_time = clock_t::now();

    while(running) {

        time_t current_time = clock_t::now();
        double dt = ( static_cast<duration_t>(current_time - previous_time) ).count() * DT_MULTIPLIER;
        previous_time = current_time;
        fps_timer += dt;

        handle_events(dt);
        update(dt);
        render();
    }
}

void yadooc::update(double dt) {

}

void yadooc::render() {

    if(fps_timer >= 1.0/FPS) {

        fps_timer = 0.0;
        window.clear();
        
        _renderer.render(&window, _map);

        window.display();
    }
}

void yadooc::handle_events(double dt) {

    while (window.pollEvent(event)) {

        if (event.type == sf::Event::Closed)
            window.close();

        switch (event.type)
        {
            case sf::Event::Closed:
                window.close();
                running = false;
                break;

            case sf::Event::KeyPressed: {
                auto it = keys_pressed.find(event.key.code);
                if(it != keys_pressed.end()) {
                    it->second =  true;
                }
                break;
            }

            case sf::Event::KeyReleased:  {
                auto it = keys_pressed.find(event.key.code);
                if(it != keys_pressed.end()) {
                    it->second =  false;
                }
                break;
            }
        }
    }

    handle_keys(dt);
}

void yadooc::handle_keys(double dt) {

    if(keys_pressed.find(sf::Keyboard::Escape)->second) {

        window.close();
        running = false;
    }
    if(keys_pressed.find(sf::Keyboard::W)->second) {

        float displacement = dt*CAMERA_SPEED;
        _camera.pos += _camera.direction * displacement;
    }
    if(keys_pressed.find(sf::Keyboard::A)->second) {
        
        float displacement = dt*CAMERA_SPEED;
        sf::Vector2f camera_dir_2f = {_camera.direction.x, _camera.direction.y};
        sf::Vector2f rotated_camera_dir_2f = rotate_vector(camera_dir_2f, -M_PI/2.f);
        sf::Vector3f rotated_camera_dir_3f = { rotated_camera_dir_2f.x, rotated_camera_dir_2f.y, _camera.direction.z };
        _camera.pos += rotated_camera_dir_3f * displacement;
    }
    if(keys_pressed.find(sf::Keyboard::S)->second) {
        
        float displacement = -dt*CAMERA_SPEED;
        _camera.pos += _camera.direction * displacement;
    }
    if(keys_pressed.find(sf::Keyboard::D)->second) {
        

        float displacement = dt*CAMERA_SPEED;
        sf::Vector2f camera_dir_2f = {_camera.direction.x, _camera.direction.y};
        sf::Vector2f rotated_camera_dir_2f = rotate_vector(camera_dir_2f, M_PI/2.f);
        sf::Vector3f rotated_camera_dir_3f = { rotated_camera_dir_2f.x, rotated_camera_dir_2f.y, _camera.direction.z };
        _camera.pos += rotated_camera_dir_3f * displacement;
    }
    if(keys_pressed.find(sf::Keyboard::Left)->second) {

        float rot_angle = -dt * CAMERA_ROT_SPEED;
        sf::Vector2f camera_dir_2f = {_camera.direction.x, _camera.direction.y};
        camera_dir_2f = rotate_vector(camera_dir_2f, rot_angle);
        _camera.direction.x = camera_dir_2f.x;
        _camera.direction.y = camera_dir_2f.y;
    }
    if(keys_pressed.find(sf::Keyboard::Right)->second) {

        float rot_angle = dt * CAMERA_ROT_SPEED;
        sf::Vector2f camera_dir_2f = {_camera.direction.x, _camera.direction.y};
        camera_dir_2f = rotate_vector(camera_dir_2f, rot_angle);
        _camera.direction.x = camera_dir_2f.x;
        _camera.direction.y = camera_dir_2f.y;
    }
    if(keys_pressed.find(sf::Keyboard::Space)->second) {

        float displacement = dt*CAMERA_ROT_SPEED;
        _camera.pos.z += displacement;
    }
    if(keys_pressed.find(sf::Keyboard::LControl)->second) {

        float displacement = -dt*CAMERA_ROT_SPEED;
        _camera.pos.z += displacement;
    }
    if(keys_pressed.find(sf::Keyboard::Up)->second) {

        float rot_angle = dt * CAMERA_ROT_SPEED;
        _camera.vertical_rot_angle += rot_angle;
        if(_camera.vertical_rot_angle > M_PI/4.f)
            _camera.vertical_rot_angle = M_PI/4.f;
    }
    if(keys_pressed.find(sf::Keyboard::Down)->second) {

        float rot_angle = -dt * CAMERA_ROT_SPEED;
        _camera.vertical_rot_angle += rot_angle;
        if(_camera.vertical_rot_angle < -M_PI/4.f)
            _camera.vertical_rot_angle = -M_PI/4.f;
    }
}