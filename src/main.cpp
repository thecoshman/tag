#include <map>

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


struct CubeCoord{
    int x, y, z;

    static CubeCoord fromGlmVec3(const glm::vec3& realPosition){
        int x = static_cast<int>(std::floor(realPosition.x));
        int y = static_cast<int>(std::ceil(realPosition.y));
        int z = static_cast<int>(std::floor(realPosition.z));
        return CubeCoord{x, y, z};
    }
};

bool operator<(const CubeCoord& lhs, const CubeCoord& rhs){
    return lhs.x < rhs.x || lhs.y < rhs.y || lhs.z < rhs.z;
}

struct Cube{
    static glm::mat4 getModelMatrix(CubeCoord coord){
        return glm::translate(glm::mat4(1.0f), glm::vec3(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5));
    }
    
    Cube(std::string textureName): textureName(textureName){};
    std::string textureName;
};

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

template<typename V, typename T = typename V::value_type>
std::pair<std::map<CubeCoord, Cube>::const_iterator, std::pair<T, T>> findClosestHit(const util::RAY<V>& ray, const std::map<CubeCoord, Cube>& worldGrid){
    T max_t = std::numeric_limits<T>::max();
    std::pair<T, T> hit_info_for_closest{max_t, max_t};
    auto nearest = worldGrid.end();

    for(auto ittr = worldGrid.begin(); ittr != worldGrid.end(); ittr++){
        auto& coord = ittr->first;
        auto aabb = util::AABB(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5, 1, 1, 1);

        auto collision_info = util::findEnterExitFraction(ray, aabb);
        if(collision_info.first){
            bool new_closest = (nearest == worldGrid.end()) || (collision_info.second.first < hit_info_for_closest.first);
            if(new_closest){
                nearest = ittr;
                hit_info_for_closest = collision_info.second;
            }
        }
    }
    return std::make_pair(nearest, hit_info_for_closest);
}

struct voxel_grid{
    void generate_world(){    
        const Cube red_cube_template = {"red_cube"};
        const Cube white_cube_template = {"white_cube"};
        const Cube green_cube_template = {"green_cube"};
        grid.insert({{ 0,  2,  0}, red_cube_template});
        grid.insert({{ 2,  1,  2}, red_cube_template});
        grid.insert({{ 10, 1, 10}, red_cube_template});
        grid.insert({{ 5,  1,  5}, red_cube_template});
        grid.insert({{ 5,  1,  5}, red_cube_template});
        grid.insert({{ 5,  1, -5}, green_cube_template});
        grid.insert({{ 3,  1, -5}, green_cube_template});
        grid.insert({{-3,  4, -5}, green_cube_template});

        for(int x = -100; x <= 100; x++){
            for(int z = -100; z <= 100; z++){
                // grid.insert({{ x,  1,  z}, white_cube_template});
                float height =  (util::simplex_noise(2, x, z) + 1) * 1;
                std::cout << height << " ";
                for(int y = 0; y < height; y++){
                    grid.insert({{ x,  y + 1,  z}, white_cube_template});                    
                }
            }
            std::cout << std::endl;
        }
    }

    std::map<CubeCoord, Cube> grid;
};

struct application{
    application(){
        player.position = glm::vec3(10,10,10);

        cam.pos = glm::vec3(10,1.7,-15); // average person about that tall, right?
        cam.dir = glm::normalize(glm::vec3(-10.0,0.0,15.0));

        player.is_space_free_query = [this](util::AABB const& aabb){
            auto hitCube = std::find_if(world.grid.begin(), world.grid.end(), [&aabb](const std::pair<const CubeCoord, Cube>& cube){
                auto& coord = cube.first;
                auto box = util::AABB(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5, 1, 1, 1);
                if(util::checkCollision(box, aabb)){
                    return true;
                } else {
                    return false;
                } });
            return hitCube == world.grid.end();
        };

        world.generate_world();
    }

    void keyboard_movement(){
        if(glfwGetKey('W')){
            player.move(tag::player::direction::forward);
        }
        if(glfwGetKey('S')){
            player.move(tag::player::direction::backward);
        }
        if(glfwGetKey('A')){
            player.move(tag::player::direction::left);
        }
        if(glfwGetKey('D')){
            player.move(tag::player::direction::right);
        }
        if(glfwGetKey(' ')){
            player.jump();
        }
    }

    void mouse_movement(){
        auto mouseDelta = window.mouse_delta();
        window.centre_mouse();
        cam.rotateYaw(mouseDelta.x / 10);
        cam.rotatePitch(-(mouseDelta.y / 10));
        player.view_vector = cam.dir;
    }

    tag::player player;
    util::Camera cam;
    voxel_grid world;
    util::glfw_window window;
};

int main(int argc, char** argv){
    application app;

    // glfw's C api makes it too awkard to move this stuff
    if(gl::exts::var_ARB_debug_output){
        gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        gl::DebugMessageCallbackARB(DebugFunc, (void*)15);
    }
    glfwSetWindowSizeCallback([](int width, int height){
        gl::Viewport(0, 0, (GLsizei) width, (GLsizei) height);
    });
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

    const Cube red_cube_template = {"red_cube"};
    const Cube white_cube_template = {"white_cube"};
    const Cube green_cube_template = {"green_cube"};

    bool leftMouseDown = false;
    bool rightMouseDown = false;

    Cube cube_creation_template = white_cube_template;

    //Main loop
    while(!app.window.exit_requested()){
        app.mouse_movement();
        
        if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED)){
            app.window.request_exit();
        }

        {
            if(glfwGetKey('1')){
                cube_creation_template = red_cube_template;
            } else if(glfwGetKey('2')){
                cube_creation_template = green_cube_template;
            } else if(glfwGetKey('3')){
                cube_creation_template = white_cube_template;
            }

            // auto players_move = players_desired_move();
            // auto new_player_position = player.position;
            // if(players_move.x != 0.0f){
            //     auto forwards_direction = glm::normalize(glm::vec3(cam.dir.x, 0.0, cam.dir.z));
            //     new_player_position += forwards_direction * players_move.z;
            // }
            // if(players_move.z != 0.0f){
            //     auto right_direction = cam.rightVector();
            //     right_direction.y = 0.0f;
            //     right_direction = glm::normalize(right_direction);
            //     new_player_position += right_direction * players_move.x;
            // }
            // if(players_move.y != 0.0f && player.grounded){
            //     player.v_speed = players_move.y;
            //     player.grounded = false;
            // }
            // if(!player.grounded){
            //     new_player_position.y += player.v_speed;
            //     if(new_player_position.y < 0.0f){
            //         new_player_position.y = 0.0f;
            //         player.v_speed = 0.0f;
            //         player.grounded = true;
            //     } else {
            //        player.v_speed -= 0.08f;
            //     }
            // }

            // if(player.position != new_player_position){
            //     auto new_position_aabb = util::AABB(new_player_position.x, (1.72/2), new_player_position.z, 0.9, 1.72, 0.9);
            //     auto hitCube = std::find_if(worldGrid.begin(), worldGrid.end(), [&new_position_aabb](const std::pair<const CubeCoord, Cube>& cube){
            //         auto& coord = cube.first;
            //         auto box = util::AABB(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5, 1, 1, 1);
            //         if(util::checkCollision(box, new_position_aabb)){
            //             std::cout << "player = {{" << new_position_aabb.min.x << ", " << new_position_aabb.min.y << ", " << new_position_aabb.min.z << "},{" << new_position_aabb.max.x << ", " << new_position_aabb.max.y << ", " << new_position_aabb.max.z << "}}" << std::endl;
            //             std::cout << "box    = {{" << box.min.x << ", " << box.min.y << ", " << box.min.z << "},{" << box.max.x << ", " << box.max.y << ", " << box.max.z << "}}" << std::endl;
            //             return true;
            //         } else {
            //             return false;
            //         } });

            //     if(hitCube != worldGrid.end()){
            //         auto& coord = hitCube->first;
            //         // worldGrid.erase(hitCube);
            //         std::cout << "You hit a cube at (" << coord.x << ", " << coord.y << ", " << coord.z << ")! You now have " << worldGrid.size() << " cubes left to get" << std::endl;
            //         player.grounded = true;
            //     } else {
            //         player.position = new_player_position;
            //         cam.pos = player.position;
            //         cam.pos.y += 1.7; // players eyes are not in their feet... I'll do this better some other time
            //     }
            // }
        }

        if(glfwGetKey('P')){
            CubeCoord coord = CubeCoord::fromGlmVec3(app.player.position);
            printf("(%f, %f, %f) => (%i, %i, %i)\n", app.player.position.x, app.player.position.y, app.player.position.z, coord.x, coord.y, coord.z);
        }

        app.keyboard_movement();
        app.player.apply_gravity();

        if(app.world.grid.size() == 0){
            printf("    you winned!\n");
            app.window.request_exit();
        }

        if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)){
            if(!leftMouseDown){
                leftMouseDown = true;
                glm::vec3 rayDirection = glm::normalize(app.cam.dir);
                util::Ray ray{app.cam.pos, rayDirection * 30.0f};
                
                auto hitInfo = findClosestHit(ray, app.world.grid);

                if(hitInfo.first != app.world.grid.end()){
                    app.world.grid.erase(hitInfo.first);
                    auto& coord = hitInfo.first->first;
                    std::cout << "You shot the box at (" << coord.x << ", " << coord.y << ", " << coord.z << ")!" << std::endl;
                    std::cout << "    You now have " << app.world.grid.size() << " cubes left to get" << std::endl;
                }
            }
        } else {
            leftMouseDown = false;
        }

        if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)){
            if(!rightMouseDown){
                rightMouseDown = true;
                glm::vec3 rayDirection = glm::normalize(app.cam.dir);
                util::Ray ray{app.cam.pos, rayDirection * 30.0f};
                
                auto hitInfo = findClosestHit(ray, app.world.grid);

                if(hitInfo.first != app.world.grid.end()){
                    glm::vec3 collision_point = ray.source + (ray.direction * hitInfo.second.first);
                    std::cout << "Picked point at (" << collision_point.x << ", " << collision_point.y << ", " << collision_point.z << ")" << std::endl;

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
                    app.world.grid.insert({create_position, cube_creation_template});
                }
            }
        } else {
            rightMouseDown = false;
        }

        app.cam.pos = app.player.eye_point();
        display(app.cam, cubeShader, cubeVao, textures, app.world.grid);
    }
    return 0;
}