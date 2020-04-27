#include <chrono>
#include <unordered_set>

#include "util/skyBox.hpp"
#include "tag/application.hpp"
#include "util/font.hpp"
#include "util/loader/loadtexture.hpp"
#include "util/gl/texture_atlas.hpp"

void initOGLsettings(){
    gl::ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    gl::Enable(gl::TEXTURE_CUBE_MAP);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthFunc(gl::LEQUAL);
    gl::Enable(gl::CULL_FACE);
    gl::CullFace(gl::BACK);
    gl::PolygonMode(gl::FRONT, gl::FILL);
}

void initShader(gldr::Program& program){
    std::string vertexShaderCode(
        "#version 330\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "layout(location = 1) in vec2 texture_coord;\n"
        "\n"
        "out vec2 texture_coord_from_vshader;\n"
        "\n"
        "uniform mat4 mvpMat;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = mvpMat * position;\n"
        "    texture_coord_from_vshader = texture_coord;\n"
        "}\n"
    );

    std::string fragmentShaderCode(
        "#version 330\n"
        "\n"
        "in vec2 texture_coord_from_vshader;\n"
        "\n"
        "out vec4 outputColor;\n"
        "\n"
        "uniform sampler2D texture_sampler;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   outputColor = texture(texture_sampler, texture_coord_from_vshader);\n"
        "}\n"
    );

    gldr::VertexShader vertexShader(vertexShaderCode);
    if(!vertexShader.didCompile()){
        std::cout << "Cube vertex shader failed to compile\n";
    }
    gldr::FragmentShader fragmentShader(fragmentShaderCode);
    if(!fragmentShader.didCompile()){
        std::cout << "Cube fragment shader failed to compile\n";
    }
    program.attach(vertexShader, fragmentShader);
    program.link();
    if(!program.didLink()){
        std::cout << "Cube program failed to link\n    >" << program.getLog() << "\n";
    }
}

void initBufferData(gldr::indexVertexBuffer& indexBuffer, gldr::dataVertexBuffer& vertexBuffer, gldr::dataVertexBuffer& textureCoordBuffer){
    std::vector<GLuint> indexdata = {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6, 
        6, 7, 4,
        8, 9,10,
        10,11, 8,
        12,13,14,
        14,15,12,
        16,17,18,
        18,19,16,
        20,21,22,
        22,23,20,
    };

    std::vector<GLfloat> vertexPositions = {
        -0.5, -0.5, -0.5,
        -0.5, -0.5,  0.5,
        -0.5,  0.5,  0.5,
        -0.5,  0.5, -0.5,

        -0.5, -0.5,  0.5,
         0.5, -0.5,  0.5,
         0.5,  0.5,  0.5,
        -0.5,  0.5,  0.5,

         0.5, -0.5,  0.5,
         0.5, -0.5, -0.5,
         0.5,  0.5, -0.5,
         0.5,  0.5,  0.5,

         0.5, -0.5, -0.5,
        -0.5, -0.5, -0.5,
        -0.5,  0.5, -0.5,
         0.5,  0.5, -0.5,

        -0.5,  0.5, -0.5,
        -0.5,  0.5,  0.5,
         0.5,  0.5,  0.5,
         0.5,  0.5, -0.5,

        -0.5, -0.5,  0.5,
        -0.5, -0.5, -0.5,
         0.5, -0.5, -0.5,
         0.5, -0.5,  0.5,
    };
    
    std::vector<GLfloat> textureCoord = {
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,

        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        0.0, 1.0,
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,

        1.0, 0.0,
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,

        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,

        1.0, 1.0,
        1.0, 0.0,
        0.0, 0.0,
        0.0, 1.0,
    };

    indexBuffer.bufferData(indexdata);

    vertexBuffer.bufferData(vertexPositions);
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, gl::FALSE, 0, 0);
    textureCoordBuffer.bufferData(textureCoord);
    gl::EnableVertexAttribArray(1);
    gl::VertexAttribPointer(1, 2, gl::FLOAT, gl::FALSE, 0, 0);
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

    std::map<std::string, gldr::Texture2d> textures;
    textures.insert(std::make_pair("red_cube", util::loader::loadTexture("resource/texture/reference_cube.png")));
    textures.insert(std::make_pair("green_cube", util::loader::loadTexture("resource/texture/green_cube.png")));
    textures.insert(std::make_pair("white_cube", util::loader::loadTexture("resource/texture/white_cube.png")));

    std::map<std::string, std::string> texturesForAtlas;
    texturesForAtlas.insert(std::make_pair("red_cube", "resource/texture/reference_cube.png"));
    texturesForAtlas.insert(std::make_pair("green_cube", "resource/texture/green_cube.png"));
    texturesForAtlas.insert(std::make_pair("white_cube", "resource/texture/white_cube.png"));

    util::gl::TextureAtlas atlas(texturesForAtlas);

    // As a temp thing, move the texture out so that I can try rending it like a 'normal' cube
    textures.insert(std::make_pair("dev_magic", std::move(atlas.texture)));

    auto skybox = util::sky_box{{
        {gldr::textureOptions::CubeMapFace::PositiveX, "resource/texture/sky_box_E.png"},
        {gldr::textureOptions::CubeMapFace::NegativeX, "resource/texture/sky_box_W.png"},
        {gldr::textureOptions::CubeMapFace::PositiveY, "resource/texture/sky_box_D.png"},
        {gldr::textureOptions::CubeMapFace::NegativeY, "resource/texture/sky_box_U.png"},
        {gldr::textureOptions::CubeMapFace::PositiveZ, "resource/texture/sky_box_N.png"},
        {gldr::textureOptions::CubeMapFace::NegativeZ, "resource/texture/sky_box_S.png"}}};

    gldr::VertexArray cubeVao;
    gldr::Program cubeShader;
    gldr::indexVertexBuffer indexBuffer;
    gldr::dataVertexBuffer vertexBuffer;
    gldr::dataVertexBuffer textureCoordBuffer;
    initShader(cubeShader);
    cubeVao.bind();
    initBufferData(indexBuffer, vertexBuffer, textureCoordBuffer);

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
        gl::Clear(gl::COLOR_BUFFER_BIT);
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
        app.display(cubeShader, cubeVao, textures);
        skybox.render(app.cam);
        debug_font.draw("fps: " + std::to_string(fps_value), glm::vec2(0.0f, 0.0f));
        debug_font.draw("pos: (" + std::to_string(app.player.position.x) + ", " + std::to_string(app.player.position.y) + ", " + std::to_string(app.player.position.z) + ")", glm::vec2(0.0f, 30.0f));
        debug_font.draw("mean phyiscs time: " + std::to_string(mean_physics_time / 1000) + "ms", glm::vec2(0.0f, 60.0f));
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