#include <string>
#include <exception>
#include <functional>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

#include <glload/gl_3_3.hpp>
#include <glload/gl_load.hpp>
#include <GL/glfw.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/swizzle.hpp>

// shut up :'(
#define GL_TRUE                 1
#define GL_FALSE                0

#include "program.hpp"
#include "vertexArray.hpp"
#include "vertexBuffer.hpp"
#include "util/camera.hpp"

util::Camera cam;
float boxY = 0.0f;

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
        "layout(location = 0) in vec4 position;\n"
        "uniform mat4 mvpMat;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = mvpMat * position;\n"
        "}\n"
    );

    std::string fragmentShaderCode(
        "#version 330\n"
        "out vec4 outputColor;\n"
        "void main()\n"
        "{\n"
        "   outputColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
        "}\n"
    );

    gldr::VertexShader vertexShader(vertexShaderCode);
    gldr::FragmentShader fragmentShader(fragmentShaderCode);
    program.attach(vertexShader, fragmentShader);
    program.link();
}

void initBufferData(gldr::VertexArray& vao, gldr::indexVertexBuffer& indexBuffer, gldr::dataVertexBuffer& vertexBuffer){
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
    
    vao.bind();

    vertexBuffer.bufferData(vertexPositions);
    indexBuffer.bufferData(indexdata);
}

void drawBox(std::function<void(glm::mat4)> setModelMatrixLambda, glm::vec3 position){
    auto modelMatrix = glm::translate(glm::mat4(1.0f), position);
    setModelMatrixLambda(modelMatrix);
    gl::DrawElements(gl::TRIANGLES, 3 * 8, gl::UNSIGNED_INT, 0);
}

void display(gldr::Program& program, gldr::VertexArray& vao){
    gl::Clear(gl::COLOR_BUFFER_BIT);
    gl::Clear(gl::DEPTH_BUFFER_BIT);

    vao.bind();
    program.use();
    GLint mvpMat = program.getUniformLocation("mvpMat");
    auto projectViewMatrix = cam.projectionMatrix() * cam.viewMatrix();
    auto lambda = [mvpMat, projectViewMatrix](glm::mat4 modelMatrix){ 
        gl::UniformMatrix4fv(mvpMat, 1, GL_FALSE, glm::value_ptr(projectViewMatrix * modelMatrix));
    };
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, GL_FALSE, 0, 0);

    drawBox(lambda, glm::vec3(0.0f, 0.5f, 0.0f));
    drawBox(lambda, glm::vec3(2.0f, 0.5f, 2.0f));
    drawBox(lambda, glm::vec3(10.0f, 0.5f, 10.0f));

    glfwSwapBuffers();
}

void reshape (int w, int h){
    gl::Viewport(0, 0, (GLsizei) w, (GLsizei) h);
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

bool checkCollision(){
    if(cam.pos.x < -0.5f){ return false; }
    if(cam.pos.x >  0.5f){ return false; }
    if(cam.pos.z < -0.5f){ return false; }
    if(cam.pos.z >  0.5f){ return false; }
    return true;
}


int main(int argc, char** argv){
    if(!glfwInit()){
        return -1;
    }

    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
    glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, gl::TRUE);
#endif

    if(!glfwOpenWindow(800, 600, 8, 8, 8, 8, 24, 8, GLFW_WINDOW)){
        glfwTerminate();
        return -1;
    }

    if(!glload::LoadFunctions()){
        glfwTerminate();
        return -1;
    }

    glfwSetWindowTitle("TAG V2");

    if(gl::exts::var_ARB_debug_output){
        gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        gl::DebugMessageCallbackARB(DebugFunc, (void*)15);
    }

    cam.pos = glm::vec3(10,1.72,0); // average person about that tall, right?
    gldr::VertexArray vao;
    gldr::Program program;
    gldr::indexVertexBuffer indexBuffer;
    gldr::dataVertexBuffer vertexBuffer;
    initOGLsettings();
    initShader(program);
    initBufferData(vao, indexBuffer, vertexBuffer);

    glfwSetWindowSizeCallback(reshape);

    int points = 0;
    //Main loop
    while(true){
        display(program, vao);

        if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED)){
            break;
        }
        if(glfwGetKey('W')){
            cam.forward(0.1f);
        }
        if(glfwGetKey('S')){
            cam.forward(-0.1f);
        }
        if(glfwGetKey('A')){
            cam.rotateYaw(-1.5f);
        }
        if(glfwGetKey('D')){
            cam.rotateYaw(1.5f);
        }
        if(glfwGetKey('Q')){
            boxY += 0.1f;
        }
        if(glfwGetKey('E')){
            boxY -= 0.1f;
        }
        if(glfwGetKey('P')){
            printf("You are at (%f, %f, %f)", cam.pos.x, cam.pos.y, cam.pos.z);
        }

        if(checkCollision()){
            points++;
            printf("You got the box! You now have %i points\n", points);
            if(points >= 3){
                printf("    you winned!\n");
                break;
            }
            cam.pos = glm::vec3(10,0,10);
        }
    }

    glfwTerminate();
    return 0;
}