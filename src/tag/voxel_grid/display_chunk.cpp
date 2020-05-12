#include "tag/voxel_grid/display_chunk.hpp"

#include "tag/voxel_grid/coord.hpp"

namespace tag {
    namespace voxel_grid {
        display_chunk::display_chunk(std::shared_ptr<util::registry<block_type>> block_registry, const chunk_coord& coord_chunk, const data_chunk& chunk){
            for(int x = 0; x < chunk_size; ++x){
                for(int y = 0; y < chunk_size; ++y){
                    for(int z = 0; z < chunk_size; ++z){
                        auto coord_intra_chunk = intra_chunk_coord{x, y, z};
                        if(needs_rendering(block_registry, chunk, coord_intra_chunk)){
                            renderable_blocks.insert({to_world_coord(coord_chunk, coord_intra_chunk), chunk.get_block(coord_intra_chunk)});
                        }
                    }
                }
            }
        }

        bool display_chunk::needs_rendering(std::shared_ptr<util::registry<block_type>> block_registry, const data_chunk& chunk, const intra_chunk_coord& coord_intra_chunk){
            auto block = chunk.get_block(coord_intra_chunk);
            auto type_of_block = block_registry->get(block.type_id).value();

            if(type_of_block.get_flag(block_type_flag::invisible)){
                return false;
            }
            if(at_chunk_edge(coord_intra_chunk)){
                return true;
            }
            if(all_neighburs_block_los(block_registry, chunk, coord_intra_chunk)){
                return false;
            }
            return true;
        }

        bool display_chunk::at_chunk_edge(const intra_chunk_coord& coord_intra_chunk){
            if(coord_intra_chunk.x == 0 || coord_intra_chunk.x == (chunk_size - 1)){ return true; }
            if(coord_intra_chunk.y == 0 || coord_intra_chunk.y == (chunk_size - 1)){ return true; }
            if(coord_intra_chunk.z == 0 || coord_intra_chunk.z == (chunk_size - 1)){ return true; }
            return false;
        }
        
        bool display_chunk::all_neighburs_block_los(std::shared_ptr<util::registry<block_type>> block_registry, const data_chunk& chunk, const intra_chunk_coord& coord){
            auto blocks_los = [block_registry, chunk, coord](intra_chunk_coord offset){
                auto test_coord = coord + offset;
                auto block = chunk.get_block(test_coord);
                auto block_type = block_registry->get(block.type_id).value();
                return block_type.get_flag(block_type_flag::fully_blocks_los);       
            };

            return 
                blocks_los({-1, 0, 0}) &&
                blocks_los({ 1, 0, 0}) &&
                blocks_los({ 0,-1, 0}) &&
                blocks_los({ 0, 1, 0}) &&
                blocks_los({ 0, 0,-1}) &&
                blocks_los({ 0, 0, 1});
        }
    }
}