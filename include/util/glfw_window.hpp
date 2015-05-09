#pragma once

#include <glm/glm.hpp>

namespace util{ 
struct glfw_window{
    glfw_window(const glm::ivec2& size = glm::ivec2(800,600), const std::string& title = "untitled window") : size(size), title(title){
        if(!glfwInit()){
            throw std::runtime_error("glfwInit failed");
        }
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
        glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef DEBUG
        glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, gl::TRUE);
        #endif   
        auto glfwWindow = glfwOpenWindow(size.x, size.y, 8, 8, 8, 8, 24, 8, GLFW_WINDOW);
        if(!glfwWindow){
            glfwTerminate();
            throw std::runtime_error("glfwOpenWindow failed");
        }
        if(!glload::LoadFunctions()){
            glfwTerminate();
            throw std::runtime_error("glload::LoadFunctions failed");
        }

        glfwSetWindowTitle(title.c_str());
    }

    ~glfw_window(){
        glfwTerminate();
    }

    glm::ivec2 mouse_position(){
        glfwGetMousePos(&mouse.x, &mouse.y);
        return glm::ivec2(mouse);
    }

    glm::ivec2 mouse_delta() const{
        int x,y;
        glfwGetMousePos(&x, &y);
        return glm::ivec2(x - mouse.x, y - mouse.y);   
    }

    void centre_mouse(){
        mouse = size/2;
        glfwSetMousePos(mouse.x, mouse.y);
    }

    void window_resize(int width, int height){
        size.x = width;
        size.y = height;
        gl::Viewport(0, 0, (GLsizei) width, (GLsizei) height);
    }

    bool exit_requested(){
        return !run;
    }

    void request_exit(){
        run = false;
    }

    private:

    glm::ivec2 size;
    std::string title;
    glm::ivec2 mouse;
    bool run = true;
};
}
