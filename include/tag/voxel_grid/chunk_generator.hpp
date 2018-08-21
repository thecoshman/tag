#pragma once

#include "tag/block_type.hpp"
#include "tag/voxel_grid/coord.hpp"
#include "tag/voxel_grid/data_chunk.hpp"
#include "util/registry.hpp"

namespace tag {
    namespace voxel_grid {
        struct chunk_generator
        {
            const std::shared_ptr<util::registry<tag::block_type>> block_registry;

            data_chunk generate(const chunk_coord& coord_chunk) const;
        };
    }
}