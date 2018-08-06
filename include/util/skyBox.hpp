#pragma once

#include <set>
#include "textureCube.hpp"
#include "camera.hpp"

namespace util{
    struct sky_box{
        sky_box(std::set<std::pair<gldr::textureOptions::CubeMapFace, const std::string>> files){
            load_textures(files);
            init_shader();
            init_vertex_data();
        }

        void render(const util::Camera& cam){
            vao.bind();
            texture.bind();
            program.use();

            auto matrix = cam.mvpMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(50,50,50)));
            gl::UniformMatrix4fv(pvm_uniform, 1, GL_FALSE, glm::value_ptr(matrix));
            gl::DrawElements(gl::QUADS, 6, gl::UNSIGNED_INT, 0);
        }

        private:
        gldr::VertexArray vao;
        gldr::TextureCube texture;
        gldr::Program program;
        gldr::indexVertexBuffer index_buffer;
        gldr::dataVertexBuffer vertex_buffer;

        GLint pvm_uniform;
        GLint vertex_atrib_location;

        void load_textures(std::set<std::pair<gldr::textureOptions::CubeMapFace, const std::string>> files){
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

        void init_shader(){
            gldr::VertexShader vertex_shader(std::string{
                "#version 330\n"
                "\n"
                "in vec3 vertex;\n"
                "out vec3 texCoord;\n"
                "uniform mat4 PVM;\n"
                "\n"
                "void main() {\n"
                "    gl_Position = PVM * vec4(vertex, 1.0);\n"
                "    texCoord = vertex;\n"
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

        void init_vertex_data(){
            vao.bind();
            vertex_buffer.bufferData(std::vector<GLfloat>{
              -1.0,  1.0,  1.0,
              -1.0, -1.0,  1.0,
               1.0, -1.0,  1.0,
               1.0,  1.0,  1.0,
              -1.0,  1.0, -1.0,
              -1.0, -1.0, -1.0,
               1.0, -1.0, -1.0,
               1.0,  1.0, -1.0,
            });
            index_buffer.bufferData(std::vector<GLuint>{
              0, 1, 2, 3,
              3, 2, 6, 7,
              7, 6, 5, 4,
              4, 5, 1, 0,
              0, 3, 7, 4,
              1, 2, 6, 5,
            });
            gl::EnableVertexAttribArray(vertex_atrib_location);
            gl::VertexAttribPointer(vertex_atrib_location, 3, gl::FLOAT, GL_FALSE, 0, 0);
        }
    };
}