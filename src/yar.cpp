#include "yar.hpp"

#include <cmath>

yar::yar() : window(sf::VideoMode(800, 800), "yar"), _camera(),  _renderer(&_camera) {

    _renderer.load_texture("stone_wall", "../../../assets/sprites/Stone.png");

    std::vector<sf::Vector2f> edges = { 
        sf::Vector2f(3,-1), 
        sf::Vector2f(3,1), 
        sf::Vector2f(5,1),
        sf::Vector2f(5,-1),
    };
    std::vector<std::string> surface_texture_ids = {"stone_wall", "stone_wall", "stone_wall", "stone_wall"};

    prism p = {
        .edges = edges,
        .surface_texture_ids = surface_texture_ids,
        .height = 2
    };
    _map.prisms.push_back(p);

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

    keys_pressed.insert({sf::Keyboard::I, false});

}

yar::~yar() {


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
            keys_pressed.find(event.key.code)->second =  true;

            if(event.key.code == sf::Keyboard::I) {
                _renderer.iter_dir *= -1;
                printf("iter dir: %d\n", _renderer.iter_dir);
            }

            break;

        case sf::Event::KeyReleased:
            keys_pressed.find(event.key.code)->second =  false;
            break;
        }
    }

    handle_keys(dt);
}

void yar::handle_keys(double dt) {

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