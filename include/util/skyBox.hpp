#pragma once

#include <set>
#include <string>
#include <utility>

#include "util/opengl.hpp"
#include <glimg/glimg.h>

#include "util/camera.hpp"

namespace util{
    struct sky_box{
        sky_box(std::set<std::pair<gldr::textureOptions::CubeMapFace, const std::string>> files);

        void render(const util::Camera& cam);

        private:
        gldr::VertexArray vao;
        gldr::TextureCube texture;
        gldr::Program program;
        gldr::indexVertexBuffer index_buffer;
        gldr::dataVertexBuffer vertex_buffer;

        GLint pvm_uniform;
        GLint vertex_atrib_location;

        void load_textures(std::set<std::pair<gldr::textureOptions::CubeMapFace, const std::string>> files);

        void init_shader();

        void init_vertex_data();
    };
}