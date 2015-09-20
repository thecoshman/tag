#pragma once
#include "voxel_grid/coord.hpp"
#include "voxel_grid/cube_type.hpp"
#include "voxel_grid/cube_registry.hpp"

namespace voxel_grid{    
    struct data_chunk{
        data_chunk(const cube_type_registry& cube_registry) : cube_registry(cube_registry){
            clear();
        }

        void generate(const chunk_coord& coord_chunk);

        void set(const intra_chunk_coord& coord, cube_type_id type_id){
            voxel_array[index(coord)] = type_id;
        }
        
        const cube_type& get(const intra_chunk_coord& coord) const{
            return cube_registry.get(voxel_array[index(coord)]);
        }

        void clear(){
            std::fill(voxel_array.begin(), voxel_array.end(), 0);
        }

        private:
        int index(const intra_chunk_coord& coord) const{
            int x = coord.x * chunk_size;
            int y = coord.y * chunk_size * chunk_size;
            int z = coord.z;
            return x + y + z;   
        }

        std::array<cube_type_id, chunk_size * chunk_size * chunk_size> voxel_array;
        const cube_type_registry& cube_registry;
    };
}