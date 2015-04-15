#include "voxel_grid/data_chunk.hpp"
#include "simplex.hpp"

namespace voxel_grid{ 
    void data_chunk::generate(const chunk_coord& coord_chunk){
        if(coord_chunk.y != 0){
            return; // for now, can only generate chunks at y=0 :(
        }
        auto coord_world = to_world_coord(coord_chunk);

        cube_template red_cube_template("red_cube", false);
        cube_template white_cube_template("white_cube", false);
        cube_template green_cube_template("green_cube", false);

        for(int x = 0; x < chunk_size; x++){
            for(int z = 0; z < chunk_size; z++){
                // float size = 16;

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

                for (int y = 0; y < h; ++y) {
                    if(y < 0.5 * chunk_size){
                        set({ x, y,  z}, white_cube_template);
                    }
                }
            }
        }
    }
}