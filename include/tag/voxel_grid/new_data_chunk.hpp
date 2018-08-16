#pragma once

#include <vector>

#include "tag/block_instance.hpp"
#include "tag/voxel_grid/coord.hpp"

namespace tag {
    namespace voxel_grid {
        struct new_data_chunk {
            new_data_chunk(std::vector<tag::block_instance> initialised_data);

            block_instance& get_block(const intra_chunk_coord& coord);

            const block_instance& get_block(const intra_chunk_coord& coord) const;

            void set_block(const block_instance& block, const intra_chunk_coord& coord);

            private:
            std::vector<tag::block_instance> data;

            int to_index(const intra_chunk_coord& coord) const;
    
            bool in_chunk(const intra_chunk_coord& coord) const;

            void assert_in_chunk(const intra_chunk_coord& coord) const;
        };
    }
}