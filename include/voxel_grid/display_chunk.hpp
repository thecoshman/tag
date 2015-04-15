#pragma once
#include "voxel_grid/coord.hpp"
#include "voxel_grid/data_chunk.hpp"

namespace voxel_grid{
    struct display_chunk {
        void cache(const chunk_coord& coord_chunk, const data_chunk& chunk){
            for(int x = 0; x < chunk_size; x++){ for(int y = 0; y < chunk_size; y++){ for(int z = 0; z < chunk_size; z++){
                intra_chunk_coord coord_intra_chunk(x, y, z);
                if(needs_rendering(chunk, coord_intra_chunk)){
                    auto cube = chunk.get(coord_intra_chunk);
                    renderable_cubes.insert({to_world_coord(coord_chunk, coord_intra_chunk), cube});
                }
            }}}
        }

        void clear(){
            renderable_cubes.clear();
        }

        std::map<world_coord, cube_template> renderable_cubes;

        private:
        bool needs_rendering(const data_chunk& chunk, const intra_chunk_coord& coord){
            return chunk.get(coord).textureName != "empty";
        }
    };
}