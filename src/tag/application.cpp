#include "tag/application.hpp"

namespace {    
    std::vector<voxel_grid::world_coord> get_overlapping_coords(util::AABB const& aabb){
        auto min = voxel_grid::world_coord::fromGlmVec3(aabb.min);
        auto max = voxel_grid::world_coord::fromGlmVec3(aabb.max);
        std::vector<voxel_grid::world_coord> overlapping_coords;
        for(int x = min.x; x <= max.x ; x++){
            for(int y = min.y; y <= max.y ; y++){
                for(int z = min.z; z <= max.z ; z++){
                    overlapping_coords.emplace_back(x, y, z);
                }
            }
        }
        return overlapping_coords;
    }
}

namespace tag {
    application::application() : world(cube_registry){
        player.position = glm::vec3(0,50,0);

        cam.pos = glm::vec3(10,1.7,-15); // average person about that tall, right?
        cam.dir = glm::normalize(glm::vec3(-10.0,0.0,15.0));

        player.is_space_free_query = [&](util::AABB const& aabb){
            auto potential_hits = get_overlapping_coords(aabb);
            for(auto coord : potential_hits){
                if(!world.get(coord).is_flag_set(voxel_grid::cube_flags::passable)){
                    return false;
                }
            }
            return true;
        };
        register_core_cube_types();
        world.generate_world({0, 0, 0}, 5);
    }

    void application::keyboard_input(float dt){
        if(window.is_key_down(GLFW_KEY_W)){
            player.move(tag::player::direction::forward);
        }
        if(window.is_key_down(GLFW_KEY_S)){
            player.move(tag::player::direction::backward);
        }
        if(window.is_key_down(GLFW_KEY_A)){
            player.move(tag::player::direction::left);
        }
        if(window.is_key_down(GLFW_KEY_D)){
            player.move(tag::player::direction::right);
        }
        if(window.is_key_down(GLFW_KEY_SPACE)){
            player.jump();
        }

        if(window.is_key_down(GLFW_KEY_ESCAPE)){
            run = false;
        }

        if(window.is_key_down(GLFW_KEY_1)){
            block_place_selection = 1;
        } else if(window.is_key_down(GLFW_KEY_2)){
            block_place_selection = 2;
        } else if(window.is_key_down(GLFW_KEY_3)){
            block_place_selection = 3;
        }
    }

    void application::mouse_input(float dt){
        auto mouseDelta = window.mouse_delta();
        window.centre_mouse();
        cam.rotateYaw(mouseDelta.x / 10);
        cam.rotatePitch(-(mouseDelta.y / 10));
        player.view_vector = cam.dir;

        if(window.is_mouse_down(GLFW_MOUSE_BUTTON_LEFT)){
            if(!leftMouseDown){
                leftMouseDown = true;
                on_left_click_fn(*this);
            }
        } else {
            leftMouseDown = false;
        }

        if(window.is_mouse_down(GLFW_MOUSE_BUTTON_RIGHT)){
            if(!rightMouseDown){
                rightMouseDown = true;
                on_right_click_fn(*this);
            }
        } else {
            rightMouseDown = false;
        }
    }

    void application::update(float dt){
        mouse_input(dt);
        keyboard_input(dt);
        player.update(dt);
    }

    void application::display(const gldr::Program& program, const gldr::VertexArray& vao, const std::map<std::string, gldr::Texture2d>& textures){
        vao.bind();
        program.use();
        GLint mvpMat = program.getUniformLocation("mvpMat");
        auto projectViewMatrix = cam.projectionMatrix() * cam.viewMatrix();

        std::string current_texture;// = "null";

        auto coord = voxel_grid::world_coord::fromGlmVec3(player.position);
        for(auto chunk : world.get_display_chunks(coord, 2)){
            for(auto &cube_pair : chunk.renderable_cubes){
                auto modelMatrix = voxel_grid::cube_type::getModelMatrix(cube_pair.first);
                gl::UniformMatrix4fv(mvpMat, 1, gl::FALSE, glm::value_ptr(projectViewMatrix * modelMatrix));

                auto cube = cube_pair.second;
                if(cube.textureName != current_texture){
                    current_texture = cube.textureName;
                    textures.find(current_texture)->second.bind();
                }
                gl::DrawElements(gl::TRIANGLES, 3 * 12, gl::UNSIGNED_INT, 0);
            }
        }
    }

    void application::register_core_cube_types(){
        air_type = cube_registry.register_new_type({voxel_grid::cube_flags::invisible | voxel_grid::cube_flags::passable, "air", "empty"});
        block_place_selection = cube_registry.register_new_type({voxel_grid::cube_flags::fully_blocks_los, "red", "red_cube"});
        cube_registry.register_new_type({voxel_grid::cube_flags::fully_blocks_los, "white", "white_cube"});
        cube_registry.register_new_type({voxel_grid::cube_flags::fully_blocks_los, "green", "green_cube"});
    }
}