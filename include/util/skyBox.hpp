#pragma once

#include <set>
#include "textureCube.hpp"
#include "camera.hpp"

namespace util{
    struct sky_box{
        sky_box(std::set<std::pair<gldr::textureOptions::CubeMapFace, const std::string>> files){
            load_textures(files);
            init_shader();
            // UVMapUniformLocation = program.getUniformLocation("uTexture");
            // mvp_matrix = program.getUniformLocation("modelViewProjectionMatrix");
            // positionLocation = program.getAttribLocation("position");

            UVMapUniformLocation = program.getUniformLocation("uTexture");
            proj_matrix = program.getUniformLocation("uProjectionMatrix");
            view_matrix = program.getUniformLocation("uWorldToCameraMatrix");
            positionLocation = program.getAttribLocation("position");
            init_vertex_data();
        }

        void render(const util::Camera& cam){
            vao.bind();
            texture.bind();
            program.use();

            // gl::UniformMatrix4fv(mvp_matrix, 1, GL_FALSE, glm::value_ptr(cam.projectionMatrix()));
            gl::UniformMatrix4fv(proj_matrix, 1, GL_FALSE, glm::value_ptr(cam.projectionMatrix()));
            gl::UniformMatrix4fv(view_matrix, 1, GL_FALSE, glm::value_ptr(cam.viewMatrix()));
            gl::DrawElements(gl::TRIANGLES, 6, gl::UNSIGNED_INT, 0);
        }

        private:
        gldr::VertexArray vao;
        gldr::TextureCube texture;
        gldr::Program program;
        gldr::indexVertexBuffer index_buffer;
        gldr::dataVertexBuffer vertex_buffer;

        GLint UVMapUniformLocation;
        // GLint mvp_matrix;
        GLint proj_matrix;
        GLint view_matrix;
        GLint positionLocation;

        void load_textures(std::set<std::pair<gldr::textureOptions::CubeMapFace, const std::string>> files){
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

                texture.imageData(face, dim.width, dim.height,
                    gldr::textureOptions::Format::RGBA,
                    gldr::textureOptions::InternalFormat::SRGB,
                    gldr::textureOptions::DataType::UnsignedByte,
                    image.GetImageData()
                );
            });
        }

        void init_shader(){
            // std::string vertex_shader_code(
            //     "#version 330\n"
            //     "\n"
            //     "uniform mat4 uProjectionMatrix;\n"
            //     "uniform mat4 uWorldToCameraMatrix;\n"

            //     "attribute vec4 position;\n"
            //     "varying mediump vec3 vTexCoordinates;\n"
            //     "uniform mat4 modelViewProjectionMatrix;\n"
            //     "\n"
            //     "void main() {\n"
            //     "    vTexCoordinates=normalize(position.xyz);\n"
            //     "    gl_Position=modelViewProjectionMatrix * position;\n"
            //     "    gl_Position=gl_Position.xyww;\n"
            //     "}\n");

            std::string vertex_shader_code(
                "#version 330\n"
                "uniform mat4 uProjectionMatrix;\n"
                "uniform mat4 uWorldToCameraMatrix;\n"
                "\n"
                "in vec4 aPosition;\n"
                "\n"
                "smooth out vec3 eyeDirection;\n"
                "\n"
                "void main() {\n"
                "    mat4 inverseProjection = inverse(uProjectionMatrix);\n"
                "    mat3 inverseModelview = transpose(mat3(uWorldToCameraMatrix));\n"
                "    vec3 unprojected = (inverseProjection * aPosition).xyz;\n"
                "    eyeDirection = inverseModelview * unprojected;\n"
                "\n"
                "    gl_Position = aPosition;\n"
                "}\n");

            std::string fragment_shader_code(
                "#version 330\n"
                "uniform samplerCube uTexture;\n"
                "\n"
                "smooth in vec3 eyeDirection;\n"
                "\n"
                "out vec4 fragmentColor;\n"
                "\n"
                "void main() {\n"
                "    fragmentColor = texture(uTexture, eyeDirection);\n"
                "}\n");

            gldr::VertexShader vertex_shader(vertex_shader_code);
            if(!vertex_shader.didCompile()){
                std::cout << "sky box vertex shader failed to compile\n";
            }
            gldr::FragmentShader fragment_shader(fragment_shader_code);
            if(!fragment_shader.didCompile()){
                std::cout << "sky box fragment shader failed to compile\n";
            }
            program.attach(vertex_shader, fragment_shader);
            program.link();
            if(!program.didLink()){
                std::cout << "sky box program failed to link\n    >" << program.getLog() << "\n";
            }
        }

        void init_vertex_data(){
            // std::vector<GLfloat> sky_vertices{
            //      1.000000, -1.000000, -1.000000, -1.000000, -1.000000, -1.000000, -1.000000,  1.000000, -1.000000, -1.000000, 
            //      1.000000,  1.000000, -1.000000, -1.000000,  1.000000,  0.999999, -1.000001,  1.000000,  1.000000,  1.000000, 
            //     -1.000000,  1.000000,  0.999999,  1.000000,  0.999999, -1.000001,  1.000000,  1.000000, -1.000000, -1.000000, 
            //      0.999999, -1.000001,  1.000000, -1.000000, -1.000000,  1.000000, -1.000000, -1.000000,  1.000000, -1.000000, 
            //      1.000000,  1.000000, -1.000000,  1.000000, -1.000000,  1.000000,  1.000000, -1.000000, -1.000000,  1.000000, 
            //     -1.000000, -1.000000,  1.000000,  1.000000,  1.000000,  1.000000, -1.000000,  1.000000, -1.000000, -1.000000, 
            //     -1.000000,  1.000000, -1.000000,  1.000000,  0.999999,  1.000000, -1.000000,  1.000000,  1.000000,  0.999999, 
            //     -1.000001,  1.000000,  1.000000, -1.000000, -1.000000,  1.000000,  1.000000, -1.000000,  0.999999, -1.000001, 
            //      1.000000, -1.000000, -1.000000, -1.000000,  1.000000, -1.000000, -1.000000, -1.000000, -1.000000,  1.000000, 
            //     -1.000000, -1.000000, -1.000000, -1.000000, -1.000000,  1.000000, -1.000000,  1.000000, -1.000000,  1.000000, 
            //      0.999999,  1.000000,  1.000000,  1.000000, -1.000000, -1.000000,  1.000000,  1.000000
            // };
            std::vector<GLfloat> sky_vertices{-1.0,-1.0, 1.0,-1.0, 1.0,1.0, -1.0,1.0 };
            // std::vector<GLuint> sky_index{
            //      0,  1,  2,  3,  4,  5, 
            //      6,  7,  8,  9, 10, 11, 
            //     12, 13, 14, 15, 16, 17, 
            //     18, 19, 20, 21, 22, 23, 
            //     24, 25, 26, 27, 28, 29, 
            //     30, 31, 32, 33, 34, 35
            // };
            std::vector<GLuint> sky_index{0,  1,  2,  0,  2,  3 };

            vao.bind();
            vertex_buffer.bufferData(sky_vertices);
            index_buffer.bufferData(sky_index);

            gl::EnableVertexAttribArray(positionLocation);
            gl::VertexAttribPointer(positionLocation, 3, gl::FLOAT, GL_FALSE, 0, 0);
        }
    };
}