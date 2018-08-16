#pragma once

#include <memory>

#include "tag/block_type.hpp"
#include "tag/voxel_grid/chunk_generator.hpp"
#include "tag/voxel_grid/chunk_generator.hpp"
#include "tag/voxel_grid/new_chunked_voxel_grid.hpp"
#include "util/registry.hpp"

namespace tag {
    class game_world {
        using registry_block_type = std::shared_ptr<util::registry<tag::block_type>>;

        registry_block_type block_registry;

        std::vector<voxel_grid::new_chunked_voxel_grid> dimensions;
        public:
        game_world(registry_block_type block_registry);

        void update(float dt);

        int add_dimenion(voxel_grid::chunk_generator generator);

        std::vector<voxel_grid::new_display_chunk> get_display_chunks(unsigned int dimensionID, const voxel_grid::world_coord& coord_world, int range) const;
    };
}