#pragma once

#include <glm/glm.hpp>
// shut up :'(
#define GL_TRUE                 1
#define GL_FALSE                0

namespace util{ 
struct glfw_window{
    glfw_window(const glm::ivec2& size = glm::ivec2(800,600), const std::string& title = "untitled window") : size(size), title(title){
        if(!glfwInit()){
            throw std::runtime_error("glfwInit failed");
        }
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        #ifdef DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, gl::TRUE);
        #endif   
        win =  glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
        if(!win){
            glfwTerminate();
            throw std::runtime_error("glfwCreateWindow failed");
        }
        glfwMakeContextCurrent(win);
        if(!glload::LoadFunctions()){
            glfwTerminate();
            throw std::runtime_error("glload::LoadFunctions failed");
        }
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwMakeContextCurrent(win);
    }

    ~glfw_window(){
        glfwDestroyWindow(win);
        glfwTerminate();
    }

    glm::dvec2 mouse_position(){
        glfwGetCursorPos(win, &mouse.x, &mouse.y);
        return glm::dvec2(mouse);
    }

    glm::dvec2 mouse_delta() const{
        double x,y;
        glfwGetCursorPos(win, &x, &y);
        return glm::dvec2(x - mouse.x, y - mouse.y);
    }

    void centre_mouse(){
        mouse = size/2;
        glfwSetCursorPos(win, mouse.x, mouse.y);
    }

    void window_resize(int width, int height){
        size.x = width;
        size.y = height;
        gl::Viewport(0, 0, (GLsizei) width, (GLsizei) height);
    }

    bool is_key_down(int key){
        return glfwGetKey(win, key) == GLFW_PRESS;
    }

    bool is_mouse_down(int button){
        return glfwGetMouseButton(win, button) == GLFW_PRESS;
    }

    bool should_close(){
        return glfwWindowShouldClose(win);
    };

    void swap_buffers(){
        glfwSwapBuffers(win);
    }

    void set_resize_fn(std::function<void(GLFWwindow*, int, int)> fn){
        typedef void resize_fn(GLFWwindow*,int,int);
        resize_fn* fn_ptr = fn.target<resize_fn>();
        if(fn_ptr == nullptr){
            // bleh
        } else {
            glfwSetWindowSizeCallback(win, fn_ptr);
        }
    }
    private:

    GLFWwindow* win = nullptr;
    glm::ivec2 size;
    std::string title;
    glm::dvec2 mouse;
};
}
