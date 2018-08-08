#include "util/glfw_window.hpp"

#include <stdexcept>

// TODO Consider using something like 'glid' to handle the lifetime of the win pointer
namespace util{ 
    glfw_window::glfw_window(const glm::ivec2& size, const std::string& title): size(size), title(title) {
        if(!glfwInit()){
            throw std::runtime_error("glfwInit failed");
        }
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SRGB_CAPABLE, true);
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
        gl::Enable(gl::FRAMEBUFFER_SRGB);
    }

    glfw_window::~glfw_window(){
        glfwDestroyWindow(win);
        glfwTerminate();
    }

    glm::dvec2 glfw_window::mouse_position(){
        glfwGetCursorPos(win, &mouse.x, &mouse.y);
        return glm::dvec2(mouse);
    }

    glm::dvec2 glfw_window::mouse_delta() const{
        double x,y;
        glfwGetCursorPos(win, &x, &y);
        return glm::dvec2(x - mouse.x, y - mouse.y);
    }

    void glfw_window::centre_mouse(){
        mouse = size/2;
        glfwSetCursorPos(win, mouse.x, mouse.y);
    }

    void glfw_window::window_resize(int width, int height){
        size.x = width;
        size.y = height;
        gl::Viewport(0, 0, (GLsizei) width, (GLsizei) height);
    }

    bool glfw_window::is_key_down(int key){
        return glfwGetKey(win, key) == GLFW_PRESS;
    }

    bool glfw_window::is_mouse_down(int button){
        return glfwGetMouseButton(win, button) == GLFW_PRESS;
    }

    bool glfw_window::should_close(){
        return glfwWindowShouldClose(win);
    };

    void glfw_window::swap_buffers(){
        glfwSwapBuffers(win);
    }

    // void glfw_window::set_resize_fn(std::function<void(GLFWwindow*, int, int)> fn){
    //     typedef void resize_fn(GLFWwindow*,int,int);
    //     resize_fn* fn_ptr = fn.target<resize_fn>();
    //     if(fn_ptr == nullptr){
    //         // bleh
    //     } else {
    //         glfwSetWindowSizeCallback(win, fn_ptr);
    //     }
    // }
}
