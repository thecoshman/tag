#pragma once
#include <iostream>
#include "coord.hpp"
#include "util/collisionCheckers.hpp"

namespace{
    template <typename T>
    std::vector<T> make_coord_range(const T& min, const T& max){
        std::vector<T> range;
        for(int x = min.x; x <= max.x; x++){
            for(int y = min.y; y <= max.y; y++){
                for(int z = min.z; z <= max.z; z++){
                    range.emplace_back(x, y, z);
                }
            }
        }
        return range;
    }
}

namespace voxel_grid {
    struct cube_template{
        static glm::mat4 getModelMatrix(world_coord coord){
            return glm::translate(glm::mat4(1.0f), glm::vec3(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5));
        }        

        cube_template(): textureName("empty"), is_passable(true){};
        cube_template(std::string textureName): textureName(textureName){};
        cube_template(std::string textureName, bool can_move_through): textureName(textureName), is_passable(can_move_through){};

        std::string textureName;
        bool is_passable = false;
    };

    struct cube{
        int template_id;
    };

    struct data_chunk{
        void generate(const chunk_coord& coord_chunk){
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

        void set(const intra_chunk_coord& coord, const cube_template& cube){
            voxel_array[index(coord)] = cube;
        }
        
        cube_template get(const intra_chunk_coord& coord) const{
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

    struct chunked_voxel_grid{
        void generate_world(){
            for(int chunk_x = -10; chunk_x < 10; chunk_x++){ for(int chunk_y = 0; chunk_y < 1; chunk_y++){ for(int chunk_z = -10; chunk_z < 10; chunk_z++){
                chunk_coord coord_chunk(chunk_x, chunk_y, chunk_z);
                data_chunk chunk;
                chunk.generate(coord_chunk);
                chunk_data.insert({coord_chunk, chunk});
            }
        }}}

        cube_template get(const world_coord& coord_world) const{
            auto coord_chunk = from_world_coord(coord_world);
            auto chunk_search = chunk_data.find(coord_chunk.first);
            if(chunk_search == chunk_data.end()){
                return cube_template();
            } else {
                return chunk_search->second.get(coord_chunk.second);
            }
        }

        enum trace_ray_options{
            include_source = 1,
            include_empty = 2,
            include_to_end = 4
        };

        std::vector<std::pair<world_coord, cube_template>> trace_ray(const util::Ray& ray, int options = 0){
            auto starting_coord = world_coord::fromGlmVec3(ray.source);
            auto end_coord = world_coord::fromGlmVec3(ray.source + ray.direction);
            // for each axis, work out which direction (if any) you should increment to move towards the end
            // potentially, these could be ordered so that you are more likely to hit the next box with the first check
            std::vector<world_coord> advancements;
            {
                auto compare = [](int start, int end){
                    if(start > end){
                        return -1;
                    }
                    if (start < end){
                        return 1;
                    } 
                    return 0;
                };
                int diff = compare(starting_coord.x, end_coord.x);
                if(diff != 0){
                    advancements.emplace_back(diff, 0, 0);
                }
                diff = compare(starting_coord.y, end_coord.y);
                if(diff != 0){
                    advancements.emplace_back(0, diff, 0);
                }
                diff = compare(starting_coord.z, end_coord.z);
                if(diff != 0){
                    advancements.emplace_back(0, 0, diff);
                }
            }

            // quck function to actually test if the coord is the next block
            auto test_hit = [&ray](world_coord coord){
                auto aabb = util::AABB(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5, 1, 1, 1);
                auto collision_info = util::findEnterExitFraction(ray, aabb);
                return collision_info.first;
            };

            std::vector<std::pair<world_coord, cube_template>> selected_blocks;

            for(auto current_coord = starting_coord; current_coord != end_coord; ){
                auto loop_start = current_coord;
                for(auto advancement : advancements){
                    if(test_hit(current_coord + advancement)){
                        current_coord += advancement;
                        auto cube = get(current_coord);
                        if(cube.textureName != "empty" || options & trace_ray_options::include_empty){
                            // if the cube on the ray is anything but air, add it to the vector
                            selected_blocks.push_back({current_coord, cube});
                        }
                        break;
                    }
                }
                if(loop_start == current_coord){
                    break; // this just makes sure that *if* the logic is wrong, we don't get stuck looping for ever
                }
            }
            return selected_blocks;
        }

        void set(const world_coord& coord_world, const cube_template& cube){
            auto chunk_coord_pair = from_world_coord(coord_world);
            auto chunk_search = chunk_data.find(chunk_coord_pair.first);
            if(chunk_search == chunk_data.end()){
                std::cout << coord_world << " is in an ungenerated/unloaded chunk\n";
                return;
            } else {
                chunk_search->second.set(chunk_coord_pair.second, cube);
                display_chunk_cache.erase(chunk_coord_pair.first); // it's no longer valid
            }
        }

        std::vector<display_chunk> get_display_chunks(const world_coord& coord_world, int range){
            auto main_chunk_coord = from_world_coord(coord_world).first;

            auto chunk_range = make_coord_range<chunk_coord>(
                {main_chunk_coord.x - range, main_chunk_coord.y - range, main_chunk_coord.z - range}, 
                {main_chunk_coord.x + range, main_chunk_coord.y + range, main_chunk_coord.z + range});

            std::vector<display_chunk> chunks_in_range;
            for(auto coord : chunk_range){ 
                auto display_chunk_search = display_chunk_cache.find(coord);
                if(display_chunk_search != display_chunk_cache.end()){
                    chunks_in_range.push_back(display_chunk_search->second);
                } else {
                    auto data_chunk_search = chunk_data.find(coord);
                    if(data_chunk_search != chunk_data.end()){
                        display_chunk fresh_display_chunk;
                        fresh_display_chunk.cache(coord, data_chunk_search->second);
                        chunks_in_range.push_back(fresh_display_chunk);
                        display_chunk_cache.insert({coord, fresh_display_chunk});
                    }
                } 
            }
            return chunks_in_range;
        }
        private:
        std::map<chunk_coord, data_chunk> chunk_data;
        std::map<chunk_coord, display_chunk> display_chunk_cache;
    };
}