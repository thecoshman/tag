#pragma once

#include <memory>
#include <map>

#include "tag/block_type.hpp"
#include "tag/block_instance.hpp"
// #include "tag/voxel_grid/chunked_voxel_grid.hpp"
#include "tag/voxel_grid/coord.hpp"
#include "tag/voxel_grid/data_chunk.hpp"
#include "util/registry.hpp"
#include "util/opengl.hpp"

namespace tag {
    namespace voxel_grid{
        struct display_chunk {
            display_chunk(std::shared_ptr<util::registry<block_type>> block_registry, const chunk_coord& coord_chunk, const data_chunk& chunk);

            void display();

            private:
            unsigned int cubeCount = 0;
            gldr::VertexArray meshVAO;
            gldr::indexVertexBuffer indexBuffer;
            gldr::dataVertexBuffer vertexBuffer;
            gldr::dataVertexBuffer textureCoordBuffer;

            bool needs_rendering(std::shared_ptr<util::registry<block_type>> block_registry, const data_chunk& chunk, const intra_chunk_coord& coord);

            bool at_chunk_edge(const intra_chunk_coord& coord);

            bool all_neighburs_block_los(std::shared_ptr<util::registry<block_type>> block_registry, const data_chunk& chunk, const intra_chunk_coord& coord);

            std::vector<GLuint> generateCubeIndexNumbers(unsigned int offset);

            std::vector<GLfloat> generateCubeVertexPositions(const intra_chunk_coord& coord_intra_chunk);

            std::vector<GLfloat> generateCubeTextureCoords(std::shared_ptr<util::registry<block_type>> block_registry, int type_id);
        };
    }
}