#include "tag/application.hpp"

#include "tag/block_type.hpp"
#include "tag/voxel_grid/chunk_generator.hpp"
#include "util/registry.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace {
    std::vector<tag::voxel_grid::world_coord> get_overlapping_coords(util::AABB const& aabb){
        auto min = tag::voxel_grid::world_coord::fromGlmVec3(aabb.min);
        auto max = tag::voxel_grid::world_coord::fromGlmVec3(aabb.max);
        std::vector<tag::voxel_grid::world_coord> overlapping_coords;
        auto range = max - min;
        overlapping_coords.reserve(range.x + range.y + range.z);
        for(int x = min.x; x <= max.x ; ++x){
            for(int y = min.y; y <= max.y ; ++y){
                for(int z = min.z; z <= max.z ; ++z){
                    overlapping_coords.emplace_back(x, y, z);
                }
            }
        }
        return overlapping_coords;
    }
}

namespace tag {
    application::application() : selected_block_id(1) {
        player.position = glm::vec3(0,50,0);

        cam.pos = glm::vec3(10,1.7,-15); // average person about that tall, right?
        cam.dir = glm::normalize(glm::vec3(-10.0,0.0,15.0));

        player.is_space_free_query = [&](util::AABB const& aabb){
            if(!world){
                return true;
            }
            auto potential_hits = get_overlapping_coords(aabb);
            for(auto coord : potential_hits) {
                if(!world->is_passable(0, coord)){
                    return false;
                }
            }
            return true;
        };

        {
            std::map<std::string, std::string> texturesForAtlas;
            texturesForAtlas.insert(std::make_pair("red_cube", "resource/texture/reference_cube.png"));
            texturesForAtlas.insert(std::make_pair("green_cube", "resource/texture/green_cube.png"));
            texturesForAtlas.insert(std::make_pair("white_cube", "resource/texture/white_cube.png"));
            textureAtlas.load(texturesForAtlas);
        }
    }

    void application::keyboard_input(float dt){
        if(window.is_key_down(GLFW_KEY_W)){
            player.move(player::direction::forward);
        }
        if(window.is_key_down(GLFW_KEY_S)){
            player.move(player::direction::backward);
        }
        if(window.is_key_down(GLFW_KEY_A)){
            player.move(player::direction::left);
        }
        if(window.is_key_down(GLFW_KEY_D)){
            player.move(player::direction::right);
        }
        if(window.is_key_down(GLFW_KEY_SPACE)){
            player.jump();
        }

        if(window.is_key_down(GLFW_KEY_ESCAPE)){
            run = false;
        }

        if(window.is_key_down(GLFW_KEY_1)){
            selected_block_id = 1;
        } else if(window.is_key_down(GLFW_KEY_2)){
            selected_block_id = 2;
        } else if(window.is_key_down(GLFW_KEY_3)){
            selected_block_id = 3;
        } else if(window.is_key_down(GLFW_KEY_4)){
            selected_block_id = 4;
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
                if(world) {
                    auto ray = util::Ray{cam.pos, glm::normalize(cam.dir) * 5.0f};
                    world->mine_blocks(0, ray, 100);
                }
            }
        } else {
            leftMouseDown = false;
        }

        if(window.is_mouse_down(GLFW_MOUSE_BUTTON_RIGHT)){
            if(!rightMouseDown){
                rightMouseDown = true;
                if(world) {
                    auto ray = util::Ray{cam.pos, glm::normalize(cam.dir) * 5.0f};
                    world->place_block(0, ray, selected_block_id);
                }
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

    void application::display(const gldr::Program& program, const gldr::VertexArray& vao){
        vao.bind();
        program.use();
        GLint mvpMat = program.getUniformLocation("mvpMat");
        GLint uvFrom = program.getUniformLocation("uvFrom");
        GLint uvTo   = program.getUniformLocation("uvTo");
        auto projectViewMatrix = cam.projectionMatrix() * cam.viewMatrix();

        std::string current_texture;// = "null";

        auto coord = voxel_grid::world_coord::fromGlmVec3(player.position);

        if(!world){
            std::cout << "Dude, there is no world\n";
        }

        textureAtlas.bind();

        for(auto chunk : world->get_display_chunks(0, coord, 2)){
            for(auto block_coord_pair : chunk.renderable_blocks){
                auto pos = block_coord_pair.first;
                auto block = block_coord_pair.second;

                auto modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x + 0.5, pos.y - 0.5, pos.z + 0.5));
                gl::UniformMatrix4fv(mvpMat, 1, gl::FALSE, glm::value_ptr(projectViewMatrix * modelMatrix));

                std::string desired_texture;
                switch (block.type_id) {
                    case 0: desired_texture = ""; break;
                    case 1: desired_texture = "white_cube"; break;
                    case 2: desired_texture = "red_cube"; break;
                    case 3: desired_texture = "green_cube"; break;
                    case 4: desired_texture = "dev_magic"; break;
                }
                auto uvCoords = textureAtlas.getUVCoords(desired_texture);
                gl::Uniform2fv(uvFrom, 1, glm::value_ptr(glm::vec2{uvCoords.fromU, uvCoords.fromV}));
                gl::Uniform2fv(uvTo, 1, glm::value_ptr(glm::vec2{uvCoords.toU, uvCoords.toV}));

                gl::DrawElements(gl::TRIANGLES, 3 * 12, gl::UNSIGNED_INT, 0);
            }
        }
    }

    void application::load_game_world(){
        auto block_registry = std::make_shared<util::registry<block_type>>();

        // Register names of blocks
        for(auto&& name : {
            "core::air",
            "core::stone",
            "core::dirt",
            "core::grass",
            "core::dev_magic"}) {
            block_registry->register_name(name);
        }
        {
            /* block_type_flag options, for reference
               none
               can_be_replaced
               gravity
               is_solid_block
               passable
               fully_blocks_los
               invisible
            */

            // Define all the data for the blocks
            auto type = block_type{"core", "air"};
            type.set_flag(block_type_flag::can_be_replaced);
            type.set_flag(block_type_flag::passable);
            type.set_flag(block_type_flag::invisible);
            block_registry->set("core::air", type);

            type = block_type{"core", "stone"};
            type.set_flag(block_type_flag::is_solid_block);
            type.set_flag(block_type_flag::fully_blocks_los);
            type.render_type = basic_cube_render_type{"white_cube"};
            block_registry->set("core::stone", type);

            type = block_type{"core", "dirt"};
            type.set_flag(block_type_flag::is_solid_block);
            type.set_flag(block_type_flag::fully_blocks_los);
            type.render_type = basic_cube_render_type{"red_cube"};
            block_registry->set("core::dirt", type);

            type = block_type{"core", "grass"};
            type.set_flag(block_type_flag::is_solid_block);
            type.set_flag(block_type_flag::passable);
            type.render_type = basic_cube_render_type{"green_cube"};
            block_registry->set("core::grass", type);

            type = block_type{"core", "dev_magic"};
            type.set_flag(block_type_flag::is_solid_block);
            type.set_flag(block_type_flag::fully_blocks_los);
            type.render_type = basic_cube_render_type{"dev_magic"};
            block_registry->set("core::dev_magic", type);
        }

        world = std::make_unique<game_world>(block_registry);

        world->add_dimenion(voxel_grid::chunk_generator{block_registry});

        std::cout << "World totally initialised\n";
    }
}