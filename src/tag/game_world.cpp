#include "tag/game_world.hpp"

namespace tag {
    game_world::game_world(registry_block_type block_registry): block_registry(block_registry){
    }

    void game_world::update(float dt){

    }

    int game_world::add_dimenion(voxel_grid::chunk_generator generator){
        dimensions.emplace_back(generator);
        return dimensions.size();
    }

    std::vector<voxel_grid::new_display_chunk> game_world::get_display_chunks(unsigned int dimensionID, const voxel_grid::world_coord& coord_world, int range) const {
        if(dimensionID > dimensions.size()) {
            return std::vector<voxel_grid::new_display_chunk>();
        }
        if(!block_registry) {
            std::cout << "We have no block_registry\n";
        }
       return dimensions[dimensionID].get_display_chunks(block_registry, coord_world, range);
    }
}