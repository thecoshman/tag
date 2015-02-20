#include <string>
#include <exception>
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

namespace util{
    struct Camera{
        Camera(): 
            pos(0.0,0.0,0.0),
            dir(0.0,0.0,-1.0),
            up(0.0,1.0,0.0),
            aspectRatio(4.0f / 3.0f),
            fov(45.0f), near(1.0f), far(999.0f){}

        glm::mat4 projection() const{
            return glm::perspective(fov, aspectRatio, near, far); 
        }

        void translate(const glm::vec3& translation){
            pos += translation;
        }

        glm::vec3 rightVector() const {
            return glm::cross(dir, up);
        }

        void rotateYaw(float degrees) {
            dir = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), -degrees, up) * glm::vec4(dir, 0.0f));
        }

        void rotatePitch(float degrees) {
            auto right = rightVector();
            up = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), degrees, right) * glm::vec4(dir, 0.0f));
            dir = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), degrees, right) * glm::vec4(dir, 0.0f));
        }

        void rotateRoll(float degrees) {
            up = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), degrees, dir) * glm::vec4(dir, 0.0f));
        }

        glm::mat4 modelView() const {
            return glm::lookAt(pos, pos + dir, up);
        }

        void forward(float distance) {
            glm::normalize(dir);
            pos += dir * distance;
        }

        void pan(glm::vec2 panning){
            glm::normalize(dir);
            glm::normalize(up);
            auto right = rightVector();
            pos += right * -panning.x;
            pos += up * panning.y;
        }

        glm::vec3 pos, dir, up;
        float aspectRatio, fov, near, far;
    };
}

GLuint positionBufferObject;
GLuint program;
GLuint vao;
util::Camera cam;
GLint modelview, projection;

GLuint BuildShader(GLenum eShaderType, const std::string &shaderText)
{
    GLuint shader = gl::CreateShader(eShaderType);
    const char *strFileData = shaderText.c_str();
    gl::ShaderSource(shader, 1, &strFileData, NULL);

    gl::CompileShader(shader);

    GLint status;
    gl::GetShaderiv(shader, gl::COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        //With ARB_debug_output, we already get the info log on compile failure.
        if(!gl::exts::var_ARB_debug_output)
        {
            GLint infoLogLength;
            gl::GetShaderiv(shader, gl::INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            gl::GetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

            const char *strShaderType = NULL;
            switch(eShaderType)
            {
                case gl::VERTEX_SHADER: strShaderType = "vertex"; break;
                case gl::GEOMETRY_SHADER: strShaderType = "geometry"; break;
                case gl::FRAGMENT_SHADER: strShaderType = "fragment"; break;
            }

            fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
            delete[] strInfoLog;
        }
        throw std::runtime_error("Compile failure in shader.");
    }
    return shader;
}


void init()
{
    gl::ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthFunc(gl::LEQUAL);
   // gl::Enable(gl::CULL_FACE);
   // gl::CullFace(gl::BACK);
   // gl::PolygonMode(gl::FRONT, gl::FILL);

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);

    const float vertexPositions[] = {
        -1.0f, -1.0f, -5.0f,
         1.0f,  1.0f, -5.0f,
        -1.0f,  1.0f, -5.0f,
         1.0f,  1.0f, -5.0f,
        -1.0f, -1.0f, -5.0f,
         1.0f, -1.0f, -5.0f,
    };

    gl::GenBuffers(1, &positionBufferObject);
    gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferObject);
    gl::BufferData(gl::ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, gl::STATIC_DRAW);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);

    const std::string vertexShader(
        "#version 330\n"
        "layout(location = 0) in vec4 position;\n"
        "uniform mat4 ModelView;\n"
        "uniform mat4 Projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = Projection * ModelView * position;\n"
       // "   gl_Position = position;\n"
        "}\n"
        );

/*#version 150

in vec4 position;
in vec4 color;
in vec2 texture_coord;
out vec4 color_from_vshader;
out vec2 texture_coord_from_vshader;

uniform mat4 ModelView;
uniform mat4 Projection;

void main() {
    gl_Position = Projection * ModelView * position;
    color_from_vshader = color;
    texture_coord_from_vshader = texture_coord;
}*/

    const std::string fragmentShader(
        "#version 330\n"
        "out vec4 outputColor;\n"
        "void main()\n"
        "{\n"
        "   outputColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
        "}\n"
        );

    GLuint vertShader = BuildShader(gl::VERTEX_SHADER, vertexShader);
    GLuint fragShader = BuildShader(gl::FRAGMENT_SHADER, fragmentShader);

    program = gl::CreateProgram();
    gl::AttachShader(program, vertShader);
    gl::AttachShader(program, fragShader);    
    gl::LinkProgram(program);

    GLint status;
    gl::GetProgramiv (program, gl::LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        if(!gl::exts::var_ARB_debug_output)
        {
            GLint infoLogLength;
            gl::GetProgramiv(program, gl::INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            gl::GetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
            fprintf(stderr, "Linker failure: %s\n", strInfoLog);
            delete[] strInfoLog;
        }

        throw std::runtime_error("Shader could not be linked.");
    }


    // modelview = program.getUniformLocation("ModelView");
    modelview = gl::GetUniformLocation(program, "ModelView");
    // projection = program.getUniformLocation("Projection");
    projection = gl::GetUniformLocation(program, "Projection");
}

void display()
{
    // gl::ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    gl::Clear(gl::COLOR_BUFFER_BIT);

    gl::UseProgram(program);

        // modelview = program.getUniformLocation("ModelView");
    modelview = gl::GetUniformLocation(program, "ModelView");
    // projection = program.getUniformLocation("Projection");
    projection = gl::GetUniformLocation(program, "Projection");
    gl::UniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(cam.modelView()));
    gl::UniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(cam.projection()));

    gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferObject);
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, GL_FALSE, 0, 0);

    gl::DrawArrays(gl::TRIANGLES, 0, 6);

    gl::DisableVertexAttribArray(0);
    gl::UseProgram(0);

    glfwSwapBuffers();
}

void reshape (int w, int h)
{
    gl::Viewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void APIENTRY DebugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam)
{
    std::string srcName;
    switch(source)
    {
        case gl::DEBUG_SOURCE_API_ARB: srcName = "API"; break;
        case gl::DEBUG_SOURCE_WINDOW_SYSTEM_ARB: srcName = "Window System"; break;
        case gl::DEBUG_SOURCE_SHADER_COMPILER_ARB: srcName = "Shader Compiler"; break;
        case gl::DEBUG_SOURCE_THIRD_PARTY_ARB: srcName = "Third Party"; break;
        case gl::DEBUG_SOURCE_APPLICATION_ARB: srcName = "Application"; break;
        case gl::DEBUG_SOURCE_OTHER_ARB: srcName = "Other"; break;
    }

    std::string errorType;
    switch(type)
    {
        case gl::DEBUG_TYPE_ERROR_ARB: errorType = "Error"; break;
        case gl::DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: errorType = "Deprecated Functionality"; break;
        case gl::DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: errorType = "Undefined Behavior"; break;
        case gl::DEBUG_TYPE_PORTABILITY_ARB: errorType = "Portability"; break;
        case gl::DEBUG_TYPE_PERFORMANCE_ARB: errorType = "Performance"; break;
        case gl::DEBUG_TYPE_OTHER_ARB: errorType = "Other"; break;
    }

    std::string typeSeverity;
    switch(severity)
    {
        case gl::DEBUG_SEVERITY_HIGH_ARB: typeSeverity = "High"; break;
        case gl::DEBUG_SEVERITY_MEDIUM_ARB: typeSeverity = "Medium"; break;
        case gl::DEBUG_SEVERITY_LOW_ARB: typeSeverity = "Low"; break;
    }

    printf("%s from %s,\t%s priority\nMessage: %s\n",
        errorType.c_str(), srcName.c_str(), typeSeverity.c_str(), message);
}

int main(int argc, char** argv)
{
    if(!glfwInit())
    {
        return -1;
    }

    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
    glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, gl::TRUE);
#endif

    if(!glfwOpenWindow(500, 500, 8, 8, 8, 8, 24, 8, GLFW_WINDOW))
    {
        glfwTerminate();
        return -1;
    }

    if(!glload::LoadFunctions())
    {
        glfwTerminate();
        return -1;
    }

    glfwSetWindowTitle("GLFW Demo");

    if(gl::exts::var_ARB_debug_output)
    {
        gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        gl::DebugMessageCallbackARB(DebugFunc, (void*)15);
    }

    init();

    glfwSetWindowSizeCallback(reshape);

    //Main loop
    while(true)
    {
        display();

        if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
        {
            break;
        }
        if(glfwGetKey('W'))
        {
            cam.forward(0.1);
        }
        if(glfwGetKey('S'))
        {
            cam.forward(-0.1);
        }
        if(glfwGetKey('A'))
        {
            cam.rotateYaw(-1);
        }
        if(glfwGetKey('D'))
        {
            cam.rotateYaw(1);
        }
    }

    glfwTerminate();
    return 0;
}