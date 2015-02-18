#include <string>
#include <exception>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

#include <glload/gl_3_3.hpp>
#include <glload/gl_load.hpp>
#include <GL/glfw.h>

// shut up :'(
#define GL_TRUE                 1
#define GL_FALSE                0

GLuint positionBufferObject;
GLuint program;
GLuint vao;

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
    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);

    const float vertexPositions[] = {
        0.75f, 0.75f, 0.0f, 1.0f,
        0.75f, -0.75f, 0.0f, 1.0f,
        -0.75f, -0.75f, 0.0f, 1.0f,
    };

    gl::GenBuffers(1, &positionBufferObject);
    gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferObject);
    gl::BufferData(gl::ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, gl::STATIC_DRAW);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);

    const std::string vertexShader(
        "#version 330\n"
        "layout(location = 0) in vec4 position;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n"
        );

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
}

void display()
{
    gl::ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    gl::Clear(gl::COLOR_BUFFER_BIT);

    gl::UseProgram(program);

    gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferObject);
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 4, gl::FLOAT, GL_FALSE, 0, 0);

    gl::DrawArrays(gl::TRIANGLES, 0, 3);

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
    }

    glfwTerminate();
    return 0;
}