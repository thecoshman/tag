#pragma once
#include "voxel_grid/coord.hpp"
#include "voxel_grid/cube_template.hpp"

namespace voxel_grid{    
    struct data_chunk{
        void generate(const chunk_coord& coord_chunk);

        void set(const intra_chunk_coord& coord, const cube_template& cube){
            voxel_array[index(coord)] = cube;
        }
        
        const cube_template& get(const intra_chunk_coord& coord) const{
            return voxel_array[index(coord)];
        }

        void clear(){
            std::fill(voxel_array.begin(), voxel_array.end(), cube_template());
        }

        private:
        int index(const intra_chunk_coord& coord) const{
            int x = coord.x * chunk_size;
            int y = coord.y * chunk_size * chunk_size;
            int z = coord.z;
            return x + y + z;   
        }

        std::array<cube_template, chunk_size * chunk_size * chunk_size> voxel_array;
    };
}