#pragma once

#include <memory>

#include "util/opengl.hpp"

#include "tag/player.hpp"
#include "tag/game_world.hpp"
#include "util/camera.hpp"
#include "util/glfw_window.hpp"

namespace tag {
    struct application{
        application();

        void keyboard_input(float dt);

        void mouse_input(float dt);

        void update(float dt);

        void display(const gldr::Program& program, const gldr::VertexArray& vao, const std::map<std::string, gldr::Texture2d>& textures);

        void load_game_world();

        glm::ivec2 window_size = glm::ivec2(800, 600);
        util::glfw_window window = util::glfw_window(window_size, "TAG V7");
        tag::player player;
        util::Camera cam;

        bool run = true;
        bool leftMouseDown = false;
        bool rightMouseDown = false;

        int selected_block_id;

        std::unique_ptr<tag::game_world> world;
    };
}