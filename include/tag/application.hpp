#pragma once

#include <memory>

#include "util/opengl.hpp"

#include "tag/player.hpp"
#include "tag/game_world.hpp"
#include "tag/voxel_grid/chunked_voxel_grid.hpp"
#include "tag/voxel_grid/cube_registry.hpp"
#include "util/camera.hpp"
#include "util/glfw_window.hpp"

namespace tag {
    struct application{
        application();

        void keyboard_input(float dt);

        void mouse_input(float dt);

        void update(float dt);

        void display(const gldr::Program& program, const gldr::VertexArray& vao, const std::map<std::string, gldr::Texture2d>& textures);

        void register_core_cube_types();

        void load_game_world();

        glm::ivec2 window_size = glm::ivec2(800, 600);
        util::glfw_window window = util::glfw_window(window_size, "TAG V6");
        tag::player player;
        util::Camera cam;
        tag::voxel_grid::chunked_voxel_grid world;
        bool run = true;
        bool leftMouseDown = false;
        bool rightMouseDown = false;

        std::function<void(application&)> on_left_click_fn = [](application&){};
        std::function<void(application&)> on_right_click_fn = [](application&){};

        tag::voxel_grid::cube_type_registry cube_registry;
        tag::voxel_grid::cube_type_id air_type;
        tag::voxel_grid::cube_type_id block_place_selection;

        std::unique_ptr<tag::game_world> new_world;
    };
}