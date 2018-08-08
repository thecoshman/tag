#pragma once

#include <string>

#include "util/opengl.hpp"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace util{
    struct glfw_window{
        glfw_window(const glm::ivec2& size = glm::ivec2(800,600), const std::string& title = "untitled window");

        ~glfw_window();

        glm::dvec2 mouse_position();

        glm::dvec2 mouse_delta() const;

        void centre_mouse();

        void window_resize(int width, int height);

        bool is_key_down(int key);

        bool is_mouse_down(int button);

        bool should_close();

        void swap_buffers();

        // void set_resize_fn(std::function<void(GLFWwindow*, int, int)> fn);
        private:

        GLFWwindow* win = nullptr;
        glm::ivec2 size;
        std::string title;
        glm::dvec2 mouse;
    };
}
