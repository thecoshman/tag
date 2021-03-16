#include <chrono>
#include <unordered_set>

#include "util/skyBox.hpp"
#include "tag/application.hpp"
#include "util/font.hpp"
#include "util/loader/loadtexture.hpp"

void initOGLsettings(){
    gl::ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    gl::Enable(gl::TEXTURE_CUBE_MAP);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthFunc(gl::LEQUAL);
    gl::Enable(gl::CULL_FACE);
    gl::CullFace(gl::BACK);
    gl::PolygonMode(gl::FRONT, gl::FILL);
}


void APIENTRY DebugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam){
    std::string srcName;
    switch(source){
        case gl::DEBUG_SOURCE_API_ARB: srcName = "API"; break;
        case gl::DEBUG_SOURCE_WINDOW_SYSTEM_ARB: srcName = "Window System"; break;
        case gl::DEBUG_SOURCE_SHADER_COMPILER_ARB: srcName = "Shader Compiler"; break;
        case gl::DEBUG_SOURCE_THIRD_PARTY_ARB: srcName = "Third Party"; break;
        case gl::DEBUG_SOURCE_APPLICATION_ARB: srcName = "Application"; break;
        case gl::DEBUG_SOURCE_OTHER_ARB: srcName = "Other"; break;
    }

    std::string errorType;
    switch(type){
        case gl::DEBUG_TYPE_ERROR_ARB: errorType = "Error"; break;
        case gl::DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: errorType = "Deprecated Functionality"; break;
        case gl::DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: errorType = "Undefined Behavior"; break;
        case gl::DEBUG_TYPE_PORTABILITY_ARB: errorType = "Portability"; break;
        case gl::DEBUG_TYPE_PERFORMANCE_ARB: errorType = "Performance"; break;
        case gl::DEBUG_TYPE_OTHER_ARB: errorType = "Other"; break;
    }

    std::string typeSeverity;
    switch(severity){
        case gl::DEBUG_SEVERITY_HIGH_ARB: typeSeverity = "High"; break;
        case gl::DEBUG_SEVERITY_MEDIUM_ARB: typeSeverity = "Medium"; break;
        case gl::DEBUG_SEVERITY_LOW_ARB: typeSeverity = "Low"; break;
    }

    std::cout << errorType << " from " << srcName << ",\t" << typeSeverity << "priority\n";
    std::cout << "Message: " << message << "\n";
}

template<typename T>
float to_microseconds(T time_point){
    return std::chrono::duration_cast<std::chrono::microseconds>(time_point).count();
}

int main(int argc, char** argv){
    using clock = std::chrono::high_resolution_clock;
    auto old_time = clock::now();

    tag::application app;
    {
        // glfw's C api makes it too awkard to move this stuff
        if(gl::exts::var_ARB_debug_output){
            gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            gl::DebugMessageCallbackARB(DebugFunc, (void*)15);
        }
        // app.window.set_resize_fn([](GLFWwindow* win, int width, int height){
        //     gl::Viewport(0, 0, (GLsizei) width, (GLsizei) height);
        // });

        glfwSetErrorCallback([](int error, const char* description){
            std::cout << std::string(description) << std::endl;
        });
        glfwSwapInterval(1);
    }
    initOGLsettings();

    app.window.centre_mouse();
    
    app.load_game_world();

    auto skybox = util::sky_box{{
        {gldr::textureOptions::CubeMapFace::PositiveX, "resource/texture/sky_box_E.png"},
        {gldr::textureOptions::CubeMapFace::NegativeX, "resource/texture/sky_box_W.png"},
        {gldr::textureOptions::CubeMapFace::PositiveY, "resource/texture/sky_box_D.png"},
        {gldr::textureOptions::CubeMapFace::NegativeY, "resource/texture/sky_box_U.png"},
        {gldr::textureOptions::CubeMapFace::PositiveZ, "resource/texture/sky_box_N.png"},
        {gldr::textureOptions::CubeMapFace::NegativeZ, "resource/texture/sky_box_S.png"}}};

    auto new_time = clock::now();
    auto delta_time = new_time - old_time;
    float startup_time = to_microseconds(delta_time);
    std::cout << "app took " << startup_time/1000 << " milliseconds to start." << std::endl;

    const float physics_step = 1000.0f / 120;
    std::cout << "physics_step set to: " << physics_step << std::endl;
    auto temporal_accumulator = 0.0f;

    font debug_font("consolas");
    auto frames_drawn = 0;
    auto timers_display_counter = 0.0f;
    auto fps_value = 0.0f;

    auto physics_timer = clock::now();
    std::array<float, 100> physics_timings;
    size_t physics_timing_index = 0;
    auto mean_physics_time = 0.0f;

    auto render_timer = clock::now();
    std::array<float, 100> render_timings;
    size_t render_timing_index = 0;
    auto mean_render_time = 0.0f;

    //Main loop
    while(!app.window.should_close() && app.run){
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        old_time = new_time;
        new_time = clock::now();
        delta_time = new_time - old_time;
        temporal_accumulator += (to_microseconds(delta_time)/1000);
        timers_display_counter += to_microseconds(delta_time);
        while(temporal_accumulator >  physics_step){
            temporal_accumulator -= physics_step;
            physics_timer = clock::now();
            app.update(physics_step);
            physics_timings[physics_timing_index++] = to_microseconds(clock::now() - physics_timer);
            if(physics_timing_index >= physics_timings.size()){
                physics_timing_index = 0;
            }
        }
        
        app.cam.pos = app.player.eye_point();

        if(timers_display_counter > 1000000.0f){ // update the timers once per second
            fps_value = frames_drawn/timers_display_counter*1000000;
            frames_drawn = 0;
            timers_display_counter -= 1000000.0f;
            mean_physics_time = 0.0f;
            for(auto time : physics_timings){
                mean_physics_time += time;
            }
            mean_physics_time /= physics_timings.size();
            mean_render_time = 0.0f;
            for(auto time : render_timings){
                mean_render_time += time;
            }
            mean_render_time /= render_timings.size();
            fps_value = 1 / mean_render_time *  1000000.0f;
        }

        render_timer = clock::now();
        app.display();
        skybox.render(app.cam);
        debug_font.draw("fps: " + std::to_string(fps_value), glm::vec2(0.0f, 0.0f));
        debug_font.draw("pos: (" + std::to_string(app.player.position.x) + ", " + std::to_string(app.player.position.y) + ", " + std::to_string(app.player.position.z) + ")", glm::vec2(0.0f, 30.0f));
        debug_font.draw("mean physics time: " + std::to_string(mean_physics_time / 1000) + "ms", glm::vec2(0.0f, 60.0f));
        debug_font.draw("mean render time:  " + std::to_string(mean_render_time / 1000) + "ms", glm::vec2(0.0f, 90.0f));
        render_timings[render_timing_index++] = to_microseconds(clock::now() - render_timer);
        if(render_timing_index >= render_timings.size()){
            render_timing_index = 0;
        }

        ++frames_drawn;
        app.window.swap_buffers();
        glfwPollEvents();
    }

    return 0;
}