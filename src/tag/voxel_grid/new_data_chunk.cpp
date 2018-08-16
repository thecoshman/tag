#include "tag/voxel_grid/new_data_chunk.hpp"

#include <stdexcept>

namespace tag {
    namespace voxel_grid {
        new_data_chunk::new_data_chunk(std::vector<tag::block_instance> initialised_data): data(std::move(initialised_data)) {
        }

        block_instance& new_data_chunk::get_block(const intra_chunk_coord& coord) {
            assert_in_chunk(coord);
            return data[to_index(coord)];
        }

        const block_instance& new_data_chunk::get_block(const intra_chunk_coord& coord) const {            
            assert_in_chunk(coord);
            return data[to_index(coord)];
        }

        void new_data_chunk::set_block(const block_instance& block, const intra_chunk_coord& coord) {
            assert_in_chunk(coord);
            data[to_index(coord)] = block;
        }

        int new_data_chunk::to_index(const intra_chunk_coord& coord) const {
            int x = coord.x * chunk_size;
            int y = coord.y * chunk_size * chunk_size;
            int z = coord.z;
            return x + y + z; 
        }
    
        bool new_data_chunk::in_chunk(const intra_chunk_coord& coord) const {
            static auto min = intra_chunk_coord{0, 0, 0};
            static auto max = intra_chunk_coord{chunk_size, chunk_size, chunk_size};
            return min <= coord && coord < max;
        }

        void new_data_chunk::assert_in_chunk(const intra_chunk_coord& coord) const {
            if(!in_chunk(coord)) {
                throw std::out_of_range("Coordinate not within chunk.");
            }
        }
    }
}