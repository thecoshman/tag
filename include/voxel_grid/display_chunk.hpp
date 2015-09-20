#pragma once
#include "voxel_grid/coord.hpp"
#include "voxel_grid/data_chunk.hpp"

namespace voxel_grid{
    struct display_chunk {
        void cache(const chunk_coord& coord_chunk, const data_chunk& chunk){
            for(int x = 0; x < chunk_size; ++x){ for(int y = 0; y < chunk_size; ++y){ for(int z = 0; z < chunk_size; ++z){
                intra_chunk_coord coord_intra_chunk(x, y, z);
                auto cube = chunk.get(coord_intra_chunk);
                if(needs_rendering(cube, chunk, coord_intra_chunk)){
                    renderable_cubes.insert({to_world_coord(coord_chunk, coord_intra_chunk), cube});
                }
            }}}
        }

        void clear(){
            renderable_cubes.clear();
        }

        std::map<world_coord, cube_type> renderable_cubes;

        private:
        bool needs_rendering(const cube_type& cube, const data_chunk& chunk, const intra_chunk_coord& coord){
            if(cube.is_flag_set(cube_flags::invisible)){
                return false;
            }
            if(at_chunk_edge(coord)){
                return true;
            }
            if(all_neighburs_block_los(chunk, coord)){
                return false;
            }
            return true;
        }

        bool at_chunk_edge(const intra_chunk_coord& coord){
            if(coord.x == 0 || coord.x == (chunk_size - 1)){ return true; }
            if(coord.y == 0 || coord.y == (chunk_size - 1)){ return true; }
            if(coord.z == 0 || coord.z == (chunk_size - 1)){ return true; }
            return false;
        }

        bool all_neighburs_block_los(const data_chunk& chunk, const intra_chunk_coord& coord){
            auto neighbur = chunk.get(coord + intra_chunk_coord(-1,0,0));
            if(!neighbur.is_flag_set(cube_flags::fully_blocks_los)) { return false; }
            neighbur = chunk.get(coord + intra_chunk_coord(1,0,0));
            if(!neighbur.is_flag_set(cube_flags::fully_blocks_los)) { return false; }
            neighbur = chunk.get(coord + intra_chunk_coord(0,-1,0));
            if(!neighbur.is_flag_set(cube_flags::fully_blocks_los)) { return false; }
            neighbur = chunk.get(coord + intra_chunk_coord(0,1,0));
            if(!neighbur.is_flag_set(cube_flags::fully_blocks_los)) { return false; }
            neighbur = chunk.get(coord + intra_chunk_coord(0,0,-1));
            if(!neighbur.is_flag_set(cube_flags::fully_blocks_los)) { return false; }
            neighbur = chunk.get(coord + intra_chunk_coord(0,0,1));
            if(!neighbur.is_flag_set(cube_flags::fully_blocks_los)) { return false; }
            return true;
        }
    };
}