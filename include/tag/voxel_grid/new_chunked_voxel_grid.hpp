#pragma once

#include <iostream>
#include <map>

#include "util/collisionCheckers.hpp"

#include "tag/voxel_grid/chunk_generator.hpp"
#include "tag/voxel_grid/coord.hpp"
#include "tag/voxel_grid/new_data_chunk.hpp"
#include "tag/voxel_grid/new_display_chunk.hpp"

namespace tag {
    namespace voxel_grid {
        struct new_chunked_voxel_grid{
            new_chunked_voxel_grid(chunk_generator generator): generator(generator) {
            }

            const block_instance& get_block(const world_coord& coord_world) const;

            block_instance& get_block(const world_coord& coord_world);

            void set_block(block_instance block, const world_coord& coord_world);

            std::vector<new_display_chunk> get_display_chunks(std::shared_ptr<util::registry<tag::block_type>> block_registry, const world_coord& coord_world, int range) const;
            private:
            new_data_chunk& get_data_chunk(const chunk_coord& coord) const;

            chunk_generator generator;
            mutable std::map<chunk_coord, new_data_chunk> chunk_data;
            mutable std::map<chunk_coord, new_display_chunk> display_chunk_cache;
        };
    }
}
