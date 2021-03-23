#pragma once

#include <iostream>
#include <map>
#include <future>
#include <optional>

#include "util/collisionCheckers.hpp"

#include "tag/voxel_grid/chunk_generator.hpp"
#include "tag/voxel_grid/coord.hpp"
#include "tag/voxel_grid/data_chunk.hpp"
#include "tag/voxel_grid/display_chunk.hpp"
#include "util/camera.hpp"

namespace tag {
    namespace voxel_grid {
        struct chunked_voxel_grid{
            chunked_voxel_grid(chunk_generator generator): generator(generator) {
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
                    // "uniform vec2 uvFrom;\n"
                    // "uniform vec2 uvTo;"
                    "\n"
                    "void main()\n"
                    "{\n"
                    // "   vec2 uvScale = uvTo - uvFrom;"
                    // "   vec2 texture_coord = uvFrom + (texture_coord_from_vshader * uvScale);"
                    // "   outputColor = texture(texture_sampler, texture_coord);\n"
                    "   outputColor = texture(texture_sampler, texture_coord_from_vshader);\n"
                    "}\n"
                );

                gldr::VertexShader vertexShader(vertexShaderCode);
                if(!vertexShader.didCompile()){
                    std::cout << "chunk_shader vertex shader failed to compile\n";
                }
                gldr::FragmentShader fragmentShader(fragmentShaderCode);
                if(!fragmentShader.didCompile()){
                    std::cout << "chunk_shader fragment shader failed to compile\n";
                }
                chunk_shader.attach(vertexShader, fragmentShader);
                chunk_shader.link();
                if(!chunk_shader.didLink()){
                    std::cout << "chunked_voxel_grid program failed to link\n    >" << chunk_shader.getLog() << "\n";
                }
            }

            const std::optional<std::reference_wrapper<block_instance>> get_block(const world_coord& coord_world) const;

            std::optional<std::reference_wrapper<block_instance>> get_block(const world_coord& coord_world);

            void set_block(block_instance block, const world_coord& coord_world);

            void display_chunks(std::shared_ptr<util::registry<block_type>> block_registry, const world_coord& coord_world, int range, const util::Camera& camera) const;

            private:
            std::optional<std::reference_wrapper<data_chunk>> get_data_chunk(const chunk_coord& coord) const;
            std::optional<std::reference_wrapper<display_chunk>> get_display_chunk(std::shared_ptr<util::registry<block_type>> block_registry, const chunk_coord& coord) const;

            gldr::Program chunk_shader;

            chunk_generator generator;
            mutable std::map<chunk_coord, data_chunk> chunk_data;
            mutable std::map<chunk_coord, std::future<data_chunk>> future_chunk_data;
            mutable std::map<chunk_coord, display_chunk> display_chunk_cache;
        };
    }
}
