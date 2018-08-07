#include "voxel_grid/chunked_voxel_grid.hpp"

namespace {
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
    chunked_voxel_grid::chunked_voxel_grid(const cube_type_registry& cube_registry):
        cube_registry(cube_registry){
    }

    void chunked_voxel_grid::generate_world(const world_coord& coord_world, int range){
        auto main_chunk_coord = from_world_coord(coord_world).first;

        auto chunk_range = make_coord_range<chunk_coord>(
            {main_chunk_coord.x - range, 0, main_chunk_coord.z - range},
            {main_chunk_coord.x + range, 0, main_chunk_coord.z + range});

        for(auto coord : chunk_range){
            get_data_chunk(coord);
        }
    }

    const cube_type& chunked_voxel_grid::get(const world_coord& coord_world) const{
        auto chunk_coord_pair = from_world_coord(coord_world);
        auto chunk = get_data_chunk(chunk_coord_pair.first);
        return chunk.get(chunk_coord_pair.second);
    }

    std::vector<std::pair<world_coord, cube_type>> chunked_voxel_grid::trace_ray(const util::Ray& ray, int options){
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

        std::vector<std::pair<world_coord, cube_type>> selected_blocks;

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

    void chunked_voxel_grid::set(const world_coord& coord_world, cube_type_id type_id){
        auto chunk_coord_pair = from_world_coord(coord_world);
        auto& chunk = get_data_chunk(chunk_coord_pair.first);
        chunk.set(chunk_coord_pair.second, type_id);
        display_chunk_cache.erase(chunk_coord_pair.first); // it's no longer valid
    }

    std::vector<display_chunk> chunked_voxel_grid::get_display_chunks(const world_coord& coord_world, int range) const{
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
                auto chunk = get_data_chunk(coord);
                display_chunk fresh_display_chunk;
                fresh_display_chunk.cache(coord, chunk);
                chunks_in_range.push_back(fresh_display_chunk);
                display_chunk_cache.insert({coord, fresh_display_chunk});
            }
        }
        return chunks_in_range;
    }

    data_chunk& chunked_voxel_grid::get_data_chunk(const chunk_coord& coord) const{
        auto chunk_search = chunk_data.find(coord);
        if(chunk_search == chunk_data.end()){
            data_chunk chunk(cube_registry);
            chunk.generate(coord);
            chunk_data.insert({coord, chunk});
        }
        try{
            return chunk_data.at(coord);
        }
        catch (const std::exception &e){
            std::cout << "oh noes, (" << coord.x << ", " << coord.y << ", " << coord.z << ")" << std::endl;
            std::cout << e.what() << std::endl;
            throw e;
        }
    }
}