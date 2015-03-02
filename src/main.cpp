#include <string>
#include <exception>
#include <stdexcept>
#include <functional>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <set>
#include <utility>

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

struct Cube{
    glm::mat4 getModelMatrix() const{
        return glm::translate(glm::mat4(1.0f), position);
    }

    glm::vec3 position;
    std::string textureName;
};

struct Window{
    Window() : size(800,600), windowTitle("TAG V3"){
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

        glfwSetWindowTitle(windowTitle.c_str());
    }

    glm::ivec2 mousePosition(){
        glfwGetMousePos(&mousePos.x, &mousePos.y);
        return glm::ivec2(mousePos);
    }

    glm::ivec2 mouseDelta(){
        int x,y;
        glfwGetMousePos(&x, &y);
        return glm::ivec2(x - mousePos.x, y - mousePos.y);   
    }

    void centreMouse(){
        // Reset mouse position for next frame
        glfwSetMousePos(size.x/2, size.y/2);
        mousePos.x = size.x/2;
        mousePos.y = size.y/2;
    }

    void windowResize(int width, int height){
        size.x = width;
        size.y = height;
        gl::Viewport(0, 0, (GLsizei) width, (GLsizei) height);
    }

    bool shouldExit(){
        return !run;
    }

    void exit(){
        run = false;
    }
private:
    glm::ivec2 size;
    std::string windowTitle;
    glm::ivec2 mousePos;
    bool run = true;
};

/*struct textureStore{
    void store(const std::string& name, gldr::Texture2d texture){

    textures.insert(std::make_pair(name, texture));
    }
    void bind(const std::string& name){
        textures.find("reference_cube")->second.bind();
    }

private:
    std::map<std::string, gldr::Texture2d> textures;
};*/

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
    auto format = image.GetFormat();
    const void* dataPtr = image.GetImageData();
    size_t  pixelSize = image.GetImageByteSize();

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

void display(const util::Camera& cam, const gldr::Program& program, const gldr::VertexArray& vao, const std::map<std::string, gldr::Texture2d>& textures, const std::vector<Cube>& cubes){
    gl::Clear(gl::COLOR_BUFFER_BIT);
    gl::Clear(gl::DEPTH_BUFFER_BIT);

    vao.bind();
    program.use();
    GLint mvpMat = program.getUniformLocation("mvpMat");
    auto projectViewMatrix = cam.projectionMatrix() * cam.viewMatrix();

    for (auto &cube : cubes){
        gl::UniformMatrix4fv(mvpMat, 1, GL_FALSE, glm::value_ptr(projectViewMatrix * cube.getModelMatrix()));
        textures.find(cube.textureName)->second.bind();
        gl::DrawElements(gl::TRIANGLES, 3 * 8, gl::UNSIGNED_INT, 0);
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

int main(int argc, char** argv){
    Window window;
    // glfw's C api makes it too awkard to move this stuff
    if(gl::exts::var_ARB_debug_output){
        gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        gl::DebugMessageCallbackARB(DebugFunc, (void*)15);
    }
    glfwSetWindowSizeCallback([](int width, int height){
        gl::Viewport(0, 0, (GLsizei) width, (GLsizei) height);
    });
    window.centreMouse();
    auto mousePos = window.mousePosition();

    glm::vec3 playerPosition = glm::vec3(10,1.7,-15);
    util::Camera cam;
    cam.pos = glm::vec3(10,1.7,-15); // average person about that tall, right?
    cam.dir = glm::normalize(glm::vec3(-10.0,0.0,15.0));

    
    std::map<std::string, gldr::Texture2d> textures;

    textures.insert(std::make_pair("red_cube", loadTexture("resource/texture/reference_cube.png")));
    textures.insert(std::make_pair("green_cube", loadTexture("resource/texture/green_cube.png")));
    textures.insert(std::make_pair("white_cube", loadTexture("resource/texture/white_cube.png")));

    gldr::VertexArray vao;
    gldr::Program program;
    gldr::indexVertexBuffer indexBuffer;
    gldr::dataVertexBuffer vertexBuffer;
    gldr::dataVertexBuffer textureCoordBuffer;
    initOGLsettings();
    initShader(program);
    vao.bind();
    initBufferData(indexBuffer, vertexBuffer, textureCoordBuffer);

    std::vector<Cube> cubes{
        Cube{ glm::vec3(0.0f, 0.5f, 0.0f), "red_cube"},
        Cube{ glm::vec3(2.0f, 0.5f, 2.0f), "red_cube"}, 
        Cube{ glm::vec3(10.0f, 0.5f, 10.0f), "red_cube"}, 
        Cube{ glm::vec3(5.0f, 0.5f, 5.0f), "red_cube"}, 
        Cube{ glm::vec3(5.0f, 1.5f, 5.0f), "red_cube"}, 
        Cube{ glm::vec3(5.0f, 0.5f, -5.0f), "green_cube"}
    };

    bool showMarker = false;
    Cube marker{ glm::vec3(0.0, 1.5, 0.0), "white_cube" };

    int points = 0;
    //Main loop
    while(!window.shouldExit()){
        auto mouseDelta = window.mouseDelta();
        window.centreMouse();

        cam.rotateYaw(mouseDelta.x / 10);
        cam.rotatePitch(-(mouseDelta.y / 10));


        if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED)){
            window.exit();
        }

        glm::vec3 playerMove = glm::vec3(0.0f, 0.0f, 0.0f); // relative to current facing
        bool actuallyMoving = false;
        if(glfwGetKey('W')){
            playerMove.z += 1.0f;
            actuallyMoving = true;
        }
        if(glfwGetKey('S')){
            playerMove.z -= 1.0f;
            actuallyMoving = true;
        }
        if(glfwGetKey('A')){
            playerMove.x -= 1.0f;
            actuallyMoving = true;
        }
        if(glfwGetKey('D')){
            playerMove.x += 1.0f;
            actuallyMoving = true;
        }
        if(actuallyMoving){
            playerMove = glm::normalize(playerMove); // avoid them moving faster when going diaganol
            playerMove *= 0.1f;
                
            glm::vec3 playerForwards = glm::normalize(glm::vec3(cam.dir.x, 0.0, cam.dir.z));
            glm::vec3 playerRight = cam.rightVector();
            playerRight.y = 0.0f;
            playerRight = glm::normalize(playerRight);
    
            playerPosition += (playerForwards * playerMove.z);
            playerPosition += playerRight * playerMove.x;
            cam.pos = playerPosition;
        }

        for (auto &cube : cubes){
            auto boxVolume = util::AABB(cube.position, 1, 1, 1);
            auto playerVolume = util::AABB(playerPosition.x, (1.72/2), playerPosition.z, 0.9, 1.72, 0.9);
            if(util::checkCollision(boxVolume, playerVolume)){
                points++;
                printf("You got the box at (%f, %f, %f)! You now have %i points\n", playerPosition.x, playerPosition.y, playerPosition.z, points);
                cube.position.y += 2;
            }
        }
        
        if(points >= 3){
            printf("    you winned!\n");
            window.exit();
        }


        if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)){
            printf("left\n");
        }
        if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)){
            printf("right\n");
        }
        std::vector<Cube> toDraw(cubes);

        display(cam, program, vao, textures, toDraw);
    }

    glfwTerminate();
    return 0;
}