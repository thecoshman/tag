#pragma once

struct CubeCoord{
    int x, y, z;

    static CubeCoord fromGlmVec3(const glm::vec3& realPosition){
        int x = static_cast<int>(std::floor(realPosition.x));
        int y = static_cast<int>(std::ceil(realPosition.y));
        int z = static_cast<int>(std::floor(realPosition.z));
        return CubeCoord{x, y, z};
    }
};

bool operator<(const CubeCoord& lhs, const CubeCoord& rhs){
    return lhs.x < rhs.x || lhs.y < rhs.y || lhs.z < rhs.z;
}

struct Cube{
    static glm::mat4 getModelMatrix(CubeCoord coord){
        return glm::translate(glm::mat4(1.0f), glm::vec3(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5));
    }
    
    Cube(std::string textureName): textureName(textureName){};
    std::string textureName;
};

template<typename V, typename T = typename V::value_type>
std::pair<std::map<CubeCoord, Cube>::const_iterator, std::pair<T, T>> findClosestHit(const util::RAY<V>& ray, const std::map<CubeCoord, Cube>& worldGrid){
    T max_t = std::numeric_limits<T>::max();
    std::pair<T, T> hit_info_for_closest{max_t, max_t};
    auto nearest = worldGrid.end();

    for(auto ittr = worldGrid.begin(); ittr != worldGrid.end(); ittr++){
        auto& coord = ittr->first;
        auto aabb = util::AABB(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5, 1, 1, 1);

        auto collision_info = util::findEnterExitFraction(ray, aabb);
        if(collision_info.first){
            bool new_closest = (nearest == worldGrid.end()) || (collision_info.second.first < hit_info_for_closest.first);
            if(new_closest){
                nearest = ittr;
                hit_info_for_closest = collision_info.second;
            }
        }
    }
    return std::make_pair(nearest, hit_info_for_closest);
}

struct voxel_grid{
    void generate_world(){    
        const Cube red_cube_template = {"red_cube"};
        const Cube white_cube_template = {"white_cube"};
        const Cube green_cube_template = {"green_cube"};
        grid.insert({{ 0,  2,  0}, red_cube_template});
        grid.insert({{ 2,  1,  2}, red_cube_template});
        grid.insert({{ 10, 1, 10}, red_cube_template});
        grid.insert({{ 5,  1,  5}, red_cube_template});
        grid.insert({{ 5,  1,  5}, red_cube_template});
        grid.insert({{ 5,  1, -5}, green_cube_template});
        grid.insert({{ 3,  1, -5}, green_cube_template});
        grid.insert({{-3,  4, -5}, green_cube_template});

        for(int x = -10; x <= 10; x++){
            for(int z = -10; z <= 10; z++){
                float height =  (util::simplex_noise(2, x, z) + 1) * 1;
                for(int y = 0; y < height; y++){
                    grid.insert({{ x,  y + 1,  z}, white_cube_template});                    
                }
            }
        }
    }

    std::map<CubeCoord, Cube> grid;
};

struct application{
    application(){
        player.position = glm::vec3(10,10,10);

        cam.pos = glm::vec3(10,1.7,-15); // average person about that tall, right?
        cam.dir = glm::normalize(glm::vec3(-10.0,0.0,15.0));

        player.is_space_free_query = [this](util::AABB const& aabb){
            auto hitCube = std::find_if(world.grid.begin(), world.grid.end(), [&aabb](const std::pair<const CubeCoord, Cube>& cube){
                auto& coord = cube.first;
                auto box = util::AABB(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5, 1, 1, 1);
                if(util::checkCollision(box, aabb)){
                    return true;
                } else {
                    return false;
                } });
            return hitCube == world.grid.end();
        };

        world.generate_world();
    }

    void keyboard_input(float dt){
        if(glfwGetKey('W')){
            player.move(tag::player::direction::forward);
        }
        if(glfwGetKey('S')){
            player.move(tag::player::direction::backward);
        }
        if(glfwGetKey('A')){
            player.move(tag::player::direction::left);
        }
        if(glfwGetKey('D')){
            player.move(tag::player::direction::right);
        }
        if(glfwGetKey(' ')){
            player.jump();
        }

        if(glfwGetKey(GLFW_KEY_ESC)){
            run = false;
        }

        if(glfwGetKey('1')){
            cube_creation_template = red_cube_template;
        } else if(glfwGetKey('2')){
            cube_creation_template = green_cube_template;
        } else if(glfwGetKey('3')){
            cube_creation_template = white_cube_template;
        }
    }

    void mouse_input(float dt){
        auto mouseDelta = window.mouse_delta();
        window.centre_mouse();
        cam.rotateYaw(mouseDelta.x / 10);
        cam.rotatePitch(-(mouseDelta.y / 10));
        player.view_vector = cam.dir;

        if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)){
            if(!leftMouseDown){
                leftMouseDown = true;
                on_left_click_fn(*this);
            }
        } else {
            leftMouseDown = false;
        }

        if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)){
            if(!rightMouseDown){
                rightMouseDown = true;
                on_right_click_fn(*this);
            }
        } else {
            rightMouseDown = false;
        }
    }

    void update(float dt){        
        mouse_input(dt);
        keyboard_input(dt);
        player.update(dt);
    }

    util::glfw_window window;
    tag::player player;
    util::Camera cam;
    voxel_grid world;
    bool run = true;
    bool leftMouseDown = false;
    bool rightMouseDown = false;
    const Cube red_cube_template = {"red_cube"};
    const Cube white_cube_template = {"white_cube"};
    const Cube green_cube_template = {"green_cube"};

    Cube cube_creation_template = white_cube_template;


    std::function<void(application&)> on_left_click_fn = [](application&){};
    std::function<void(application&)> on_right_click_fn = [](application&){};
};