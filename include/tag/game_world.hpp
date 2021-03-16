#pragma once

#include <memory>

#include "tag/block_type.hpp"
#include "tag/voxel_grid/chunk_generator.hpp"
#include "tag/voxel_grid/chunk_generator.hpp"
#include "tag/voxel_grid/chunked_voxel_grid.hpp"
#include "util/registry.hpp"
#include "util/volumes.hpp"
#include "util/camera.hpp"

namespace tag {
    class game_world {
        using registry_block_type = std::shared_ptr<util::registry<block_type>>;

        registry_block_type block_registry;

        std::vector<voxel_grid::chunked_voxel_grid> dimensions;
        public:
        game_world(registry_block_type block_registry);

        void update(float dt);

        int add_dimenion(voxel_grid::chunk_generator generator);

        void display_chunks(unsigned int dimensionID, const voxel_grid::world_coord& coord_world, int range, const util::Camera& camera) const;

        void mine_blocks(unsigned int dimensionID, util::Ray& ray, int mining_power);

        void place_block(unsigned int dimensionID, util::Ray& ray, int place_id);

        bool is_passable(unsigned int dimensionID, const voxel_grid::world_coord& coord) const;
    };
}