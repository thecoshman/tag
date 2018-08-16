#pragma once

#include <iostream>
#include <map>

#include "util/collisionCheckers.hpp"

#include "tag/voxel_grid/coord.hpp"
#include "tag/voxel_grid/cube_registry.hpp"
#include "tag/voxel_grid/cube_type.hpp"
#include "tag/voxel_grid/data_chunk.hpp"
#include "tag/voxel_grid/display_chunk.hpp"

namespace tag {
    namespace voxel_grid {
        struct cube{
            int template_id;
        };

        struct chunked_voxel_grid{
            chunked_voxel_grid(const cube_type_registry& cube_registry);

            void generate_world(const world_coord& coord_world, int range);

            const cube_type& get(const world_coord& coord_world) const;

            enum trace_ray_options{
                include_source = 1,
                include_empty = 2,
                include_to_end = 4
            };

            std::vector<std::pair<world_coord, cube_type>> trace_ray(const util::Ray& ray, int options = 0);

            void set(const world_coord& coord_world, cube_type_id type_id);

            std::vector<display_chunk> get_display_chunks(const world_coord& coord_world, int range) const;

            private:
            data_chunk& get_data_chunk(const chunk_coord& coord) const;

            mutable std::map<chunk_coord, data_chunk> chunk_data;
            mutable std::map<chunk_coord, display_chunk> display_chunk_cache;
            const cube_type_registry& cube_registry;
        };
    }
}
