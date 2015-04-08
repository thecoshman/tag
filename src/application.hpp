#pragma once

#include "voxel_grid.hpp"
#include <stdexcept>


namespace{
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

struct application{
    application(){
        player.position = glm::vec3(0,50,0);

        cam.pos = glm::vec3(10,1.7,-15); // average person about that tall, right?
        cam.dir = glm::normalize(glm::vec3(-10.0,0.0,15.0));

        player.is_space_free_query = [&](util::AABB const& aabb){
            auto potential_hits = get_overlapping_coords(aabb);
            for(auto coord : potential_hits){
                if(!world.get(coord).is_passable){
                    return false;
                }
            }
            return true;
        };

        world.generate_world({0, 0, 0}, 5);
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

    void display(const gldr::Program& program, const gldr::VertexArray& vao, const std::map<std::string, gldr::Texture2d>& textures){
        gl::Clear(gl::COLOR_BUFFER_BIT);
        gl::Clear(gl::DEPTH_BUFFER_BIT);

        vao.bind();
        program.use();
        GLint mvpMat = program.getUniformLocation("mvpMat");
        auto projectViewMatrix = cam.projectionMatrix() * cam.viewMatrix();

        std::string current_texture;// = "null";

        auto coord = voxel_grid::world_coord::fromGlmVec3(player.position);
        for(auto chunk : world.get_display_chunks(coord, 2)){
            for(auto &cube_pair : chunk.renderable_cubes){
                auto modelMatrix = voxel_grid::cube_template::getModelMatrix(cube_pair.first);
                gl::UniformMatrix4fv(mvpMat, 1, GL_FALSE, glm::value_ptr(projectViewMatrix * modelMatrix));

                auto cube = cube_pair.second;
                if(cube.textureName != current_texture){
                    current_texture = cube.textureName;
                    textures.find(current_texture)->second.bind();
                }
                gl::DrawElements(gl::TRIANGLES, 3 * 12, gl::UNSIGNED_INT, 0);
            }

        }

        glfwSwapBuffers();
    }

    util::glfw_window window;
    tag::player player;
    util::Camera cam;
    voxel_grid::chunked_voxel_grid world;
    bool run = true;
    bool leftMouseDown = false;
    bool rightMouseDown = false;
    const voxel_grid::cube_template red_cube_template = {"red_cube"};
    const voxel_grid::cube_template white_cube_template = {"white_cube"};
    const voxel_grid::cube_template green_cube_template = {"green_cube"};

    voxel_grid::cube_template cube_creation_template = white_cube_template;
    voxel_grid::cube_template cube_creation_template_tmp{"red_cube"};


    std::function<void(application&)> on_left_click_fn = [](application&){};
    std::function<void(application&)> on_right_click_fn = [](application&){};
};