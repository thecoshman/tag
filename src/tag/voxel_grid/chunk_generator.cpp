#include "tag/voxel_grid/chunk_generator.hpp"

#include <vector>

#include "simplex.hpp"
#include "tag/voxel_grid/voxel_grid_constants.hpp"

namespace tag {
    namespace voxel_grid {
        data_chunk chunk_generator::generate(const chunk_coord& coord_chunk) const {
            std::vector<block_instance> chunk_data;
            chunk_data.reserve(blocks_per_chunk);
            if(!block_registry) {
                std::cout << "block_registry is null\n";
            }
            auto air_id = block_registry->get_id("core::air").value();
            auto stone_id = block_registry->get_id("core::stone").value();

            auto coord_world = to_world_coord(coord_chunk);

            for (int y = 0; y < chunk_size; y++) {
                for(int x = 0; x < chunk_size; x++){
                    for(int z = 0; z < chunk_size; z++){
                        float xf = (x + coord_world.x) / (chunk_size * 6.0);
                        float zf = (z + coord_world.z) / (chunk_size * 6.0);

                        double value = util::simplex_noise(1, xf, zf);

                        //compress the range a bit:
                        value *= 0.7;
                        value += 0.2;
                        if (value > 1.0) {
                            value = 1.0;
                        }

                        int h = value * chunk_size;

                        auto is_stone = coord_chunk.y == 0 && y < 0.5 * chunk_size && y < h;
                        auto block_type_id = is_stone ? stone_id : air_id;

                        chunk_data.emplace_back(block_type_id, 0, 0, 0);
                    }
                }
            }
            return data_chunk(chunk_data);
        }
    }
}