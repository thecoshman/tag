#include <map>
#include <chrono>

#include <glload/gl_3_3.hpp>
#include <glload/gl_load.hpp>
#include <GL/glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/swizzle.hpp>

#include <glimg/glimg.h>

// shut up :'(
#define GL_TRUE                 1
#define GL_FALSE                0

#include "program.hpp"
#include "vertexArray.hpp"
#include "vertexBuffer.hpp"
#include "texture.hpp"

#include "util/camera.hpp"
#include "util/collisionCheckers.hpp"
#include "util/glfw_window.hpp"
#include "tag/player.hpp"
#include "simplex.hpp"
#include "application.hpp"

void initOGLsettings(){
    gl::ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
    gldr::FragmentShader fragmentShader(fragmentShaderCode);
    program.attach(vertexShader, fragmentShader);
    program.link();
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
    gl::VertexAttribPointer(0, 3, gl::FLOAT, GL_FALSE, 0, 0);
    textureCoordBuffer.bufferData(textureCoord);
    gl::EnableVertexAttribArray(1);
    gl::VertexAttribPointer(1, 2, gl::FLOAT, GL_FALSE, 0, 0);
}

gldr::Texture2d loadTexture(const std::string& file){
    std::unique_ptr<glimg::ImageSet> imageSet(glimg::loaders::stb::LoadFromFile(file));
    auto image = imageSet->GetImage(0);
    auto dim = image.GetDimensions();
    // auto format = image.GetFormat();
    // const void* dataPtr = image.GetImageData();
    // size_t  pixelSize = image.GetImageByteSize();

    gldr::Texture2d texture;
    texture.setFiltering(gldr::textureOptions::FilterDirection::Minification, gldr::textureOptions::FilterMode::Linear);
    texture.setFiltering(gldr::textureOptions::FilterDirection::Magnification, gldr::textureOptions::FilterMode::Linear);

    texture.imageData(dim.width, dim.height,
        gldr::textureOptions::Format::RGBA,
        gldr::textureOptions::InternalFormat::RGB,
        gldr::textureOptions::DataType::UnsignedByte,
        image.GetImageData()
    );
    return texture;
}

void display(const util::Camera& cam, const gldr::Program& program, const gldr::VertexArray& vao, const std::map<std::string, gldr::Texture2d>& textures, const std::map<CubeCoord, Cube>& worldGrid){
    gl::Clear(gl::COLOR_BUFFER_BIT);
    gl::Clear(gl::DEPTH_BUFFER_BIT);

    vao.bind();
    program.use();
    GLint mvpMat = program.getUniformLocation("mvpMat");
    auto projectViewMatrix = cam.projectionMatrix() * cam.viewMatrix();

    for (auto &pair : worldGrid){
        CubeCoord coord = pair.first;
        Cube cube = pair.second;
        auto modelMatrix = Cube::getModelMatrix(coord);
        gl::UniformMatrix4fv(mvpMat, 1, GL_FALSE, glm::value_ptr(projectViewMatrix * modelMatrix));
        textures.find(cube.textureName)->second.bind();
        gl::DrawElements(gl::TRIANGLES, 3 * 12, gl::UNSIGNED_INT, 0);
    }

    glfwSwapBuffers();
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

    printf("%s from %s,\t%s priority\nMessage: %s\n", errorType.c_str(), srcName.c_str(), typeSeverity.c_str(), message);
}

void break_block(application& app){
    glm::vec3 rayDirection = glm::normalize(app.cam.dir);
    util::Ray ray{app.cam.pos, rayDirection * 30.0f};
    
    auto hitInfo = findClosestHit(ray, app.world.grid);

    if(hitInfo.first != app.world.grid.end()){
        app.world.grid.erase(hitInfo.first);
    }
}

void place_block(application& app){
    glm::vec3 rayDirection = glm::normalize(app.cam.dir);
    util::Ray ray{app.cam.pos, rayDirection * 30.0f};
    
    auto hitInfo = findClosestHit(ray, app.world.grid);

    if(hitInfo.first != app.world.grid.end()){
        glm::vec3 collision_point = ray.source + (ray.direction * hitInfo.second.first);

        auto& hit_coord = hitInfo.first->first;
        CubeCoord create_position = hit_coord;
        if(hit_coord.x == collision_point.x){
            create_position.x--;
        } else if(hit_coord.x == collision_point.x - 1){
            create_position.x++;
        } else if(hit_coord.y == collision_point.y){
            create_position.y++;
        } else if(hit_coord.y == collision_point.y + 1){
            create_position.y--;
        }else if(hit_coord.z == collision_point.z){
            create_position.z--;
        } else if(hit_coord.z == collision_point.z - 1){
            create_position.z++;
        }
        app.world.grid.insert({create_position, app.cube_creation_template});
    }
}

template<typename T>
float to_microseconds(T time_point){
    return std::chrono::duration_cast<std::chrono::microseconds>(time_point).count();
}

int main(int argc, char** argv){
    using clock = std::chrono::high_resolution_clock;
    auto old_time = clock::now();

    application app;
    {
        // glfw's C api makes it too awkard to move this stuff
        if(gl::exts::var_ARB_debug_output){
            gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            gl::DebugMessageCallbackARB(DebugFunc, (void*)15);
        }
        glfwSetWindowSizeCallback([](int width, int height){
            gl::Viewport(0, 0, (GLsizei) width, (GLsizei) height);
        });
    }
    app.window.centre_mouse();
    
    std::map<std::string, gldr::Texture2d> textures;

    textures.insert(std::make_pair("red_cube", loadTexture("resource/texture/reference_cube.png")));
    textures.insert(std::make_pair("green_cube", loadTexture("resource/texture/green_cube.png")));
    textures.insert(std::make_pair("white_cube", loadTexture("resource/texture/white_cube.png")));

    gldr::VertexArray cubeVao;
    gldr::Program cubeShader;
    gldr::indexVertexBuffer indexBuffer;
    gldr::dataVertexBuffer vertexBuffer;
    gldr::dataVertexBuffer textureCoordBuffer;
    initOGLsettings();
    initShader(cubeShader);
    cubeVao.bind();
    initBufferData(indexBuffer, vertexBuffer, textureCoordBuffer);

    app.on_left_click_fn = break_block;
    app.on_right_click_fn = place_block;

    auto new_time = clock::now();
    auto delta_time = new_time - old_time;
    float startup_time = to_microseconds(delta_time);
    std::cout << "app took " << startup_time/1000 << " milliseconds to start." << std::endl;

    const float physics_step = 1000.0f / 120;
    std::cout << "physics_step set to: " << physics_step << std::endl;
    float temporal_accumulator = 0.0;

    //Main loop
    while(!app.window.exit_requested() && app.run){
        old_time = new_time;
        new_time = clock::now();
        delta_time = new_time - old_time;
        temporal_accumulator += (to_microseconds(delta_time)/1000);

        while(temporal_accumulator >  physics_step){
            temporal_accumulator -= physics_step;

            if(!glfwGetWindowParam(GLFW_OPENED)){
                app.window.request_exit();
            }

            app.update(physics_step);
        }

        if(app.world.grid.size() == 0){
            printf("    you winned!\n");
            app.window.request_exit();
        }

        app.cam.pos = app.player.eye_point();
        display(app.cam, cubeShader, cubeVao, textures, app.world.grid);
    }
    return 0;
}