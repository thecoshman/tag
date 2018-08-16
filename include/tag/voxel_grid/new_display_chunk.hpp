#pragma once

#include <memory>
#include <map>

#include "tag/block_type.hpp"
#include "tag/block_instance.hpp"
#include "tag/voxel_grid/coord.hpp"
#include "tag/voxel_grid/new_data_chunk.hpp"
#include "util/registry.hpp"

namespace tag {
    namespace voxel_grid{
        struct new_display_chunk {
            new_display_chunk(std::shared_ptr<util::registry<tag::block_type>> block_registry, const chunk_coord& coord_chunk, const new_data_chunk& chunk);

            void clear();

            std::map<world_coord, block_instance> renderable_blocks;

            private:
            bool needs_rendering(std::shared_ptr<util::registry<tag::block_type>> block_registry, const new_data_chunk& chunk, const intra_chunk_coord& coord);

            bool at_chunk_edge(const intra_chunk_coord& coord);

            bool all_neighburs_block_los(std::shared_ptr<util::registry<tag::block_type>> block_registry, const new_data_chunk& chunk, const intra_chunk_coord& coord);
        };
    }
}