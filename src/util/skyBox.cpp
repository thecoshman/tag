#include "util/skyBox.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace util{
    sky_box::sky_box(std::set<std::pair<gldr::textureOptions::CubeMapFace, const std::string>> files){
        load_textures(files);
        init_shader();
        init_vertex_data();
    }

    void sky_box::render(const util::Camera& cam){
        vao.bind();
        texture.bind();
        program.use();

        auto projection = cam.projectionMatrix();
        auto view = glm::lookAt(glm::vec3(0,0,0), cam.dir, cam.up);
        auto model = glm::scale(glm::mat4(1.0f), glm::vec3(100,-100,100));
        gl::UniformMatrix4fv(pvm_uniform, 1, gl::FALSE, glm::value_ptr(projection * view * model));
        gl::DrawElements(gl::TRIANGLES, 3 * 12, gl::UNSIGNED_INT, 0);
    }

    void sky_box::load_textures(std::set<std::pair<gldr::textureOptions::CubeMapFace, const std::string>> files){
        texture.bind();
        std::for_each(files.begin(), files.end(), [&](const std::pair<gldr::textureOptions::CubeMapFace, const std::string>& face_file_pair){
            auto face = face_file_pair.first;
            auto file = face_file_pair.second;

            std::unique_ptr<glimg::ImageSet> imageSet(glimg::loaders::stb::LoadFromFile(file));
            auto image = imageSet->GetImage(0);
            auto dim = image.GetDimensions();
            
            texture.setFiltering(gldr::textureOptions::FilterDirection::Minification, gldr::textureOptions::FilterMode::Linear);
            texture.setFiltering(gldr::textureOptions::FilterDirection::Magnification, gldr::textureOptions::FilterMode::Linear);
            texture.setWrapMode(gldr::textureOptions::WrapDirection::S, gldr::textureOptions::WrapMode::EdgeClamp);
            texture.setWrapMode(gldr::textureOptions::WrapDirection::T, gldr::textureOptions::WrapMode::EdgeClamp);
            texture.setWrapMode(gldr::textureOptions::WrapDirection::R, gldr::textureOptions::WrapMode::EdgeClamp);

            texture.imageData(face, dim.width, dim.height,
                gldr::textureOptions::Format::RGBA,
                gldr::textureOptions::InternalFormat::SRGB,
                gldr::textureOptions::DataType::UnsignedByte,
                image.GetImageData()
            );
        });
    }

    void sky_box::init_shader(){
        gldr::VertexShader vertex_shader(std::string{
            "#version 330\n"
            "\n"
            "in vec3 vertex;\n"
            "out vec3 texCoord;\n"
            "uniform mat4 PVM;\n"
            "\n"
            "void main() {\n"
            "    gl_Position = PVM * vec4(vertex, 1.0);\n"
            "    texCoord = vec3(-vertex.x, vertex.y, vertex.z);\n"
            "}\n"});
        if(!vertex_shader.didCompile()){
            std::cout << "sky box vertex shader failed to compile\n    >" << vertex_shader.getLog() << "\n";
        }

        gldr::FragmentShader fragment_shader(std::string{
            "#version 330\n"
            "\n"
            "in vec3 texCoord;\n"
            "out vec4 fragColor;\n"
            "uniform samplerCube cubemap;\n"
            "\n"
            "void main (void) {\n"
            "    fragColor = texture(cubemap, texCoord);\n"
            "}\n"});
        if(!fragment_shader.didCompile()){
            std::cout << "sky box fragment shader failed to compile\n    >" << fragment_shader.getLog() << "\n";
        }

        program.attach(vertex_shader, fragment_shader);
        program.link();
        if(!program.didLink()){
            std::cout << "sky box program failed to link\n    >" << program.getLog() << "\n";
        }

        pvm_uniform = program.getUniformLocation("PVM");
        vertex_atrib_location = program.getAttribLocation("vertex");
    }

    void sky_box::init_vertex_data(){
        vao.bind();

        vertex_buffer.bufferData(std::vector<GLfloat>{
            -1.0, -1.0, -1.0,
            -1.0, -1.0,  1.0,
            -1.0,  1.0,  1.0,
            -1.0,  1.0, -1.0,

            -1.0, -1.0,  1.0,
             1.0, -1.0,  1.0,
             1.0,  1.0,  1.0,
            -1.0,  1.0,  1.0,

             1.0, -1.0,  1.0,
             1.0, -1.0, -1.0,
             1.0,  1.0, -1.0,
             1.0,  1.0,  1.0,

             1.0, -1.0, -1.0,
            -1.0, -1.0, -1.0,
            -1.0,  1.0, -1.0,
             1.0,  1.0, -1.0,

            -1.0,  1.0, -1.0,
            -1.0,  1.0,  1.0,
             1.0,  1.0,  1.0,
             1.0,  1.0, -1.0,

            -1.0, -1.0,  1.0,
            -1.0, -1.0, -1.0,
             1.0, -1.0, -1.0,
             1.0, -1.0,  1.0,
        });

        index_buffer.bufferData(std::vector<GLuint>{
             0, 1,   2,
             2, 3,   0,
             4, 5,   6,
             6, 7,   4,
             8, 9,  10,
            10, 11,  8,
            12, 13, 14,
            14, 15, 12,
            16, 17, 18,
            18, 19, 16,
            20, 21, 22,
            22, 23, 20,
        });

        gl::EnableVertexAttribArray(vertex_atrib_location);
        gl::VertexAttribPointer(vertex_atrib_location, 3, gl::FLOAT, gl::FALSE, 0, 0);
    }
}