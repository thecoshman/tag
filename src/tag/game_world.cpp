#include "tag/game_world.hpp"

namespace {
    std::vector<tag::voxel_grid::world_coord> ray_to_block_coords(const util::Ray& ray){
        auto starting_coord = tag::voxel_grid::world_coord::fromGlmVec3(ray.source);
        auto end_coord = tag::voxel_grid::world_coord::fromGlmVec3(ray.source + ray.direction);
        
        // For each axis, work out which direction (if any) we should increment to move from 'starting_coord' to 'end_coord'
        // This gives us up to three (one each for the x, y and z axis) possible directions that will advance us into the next block
        std::vector<tag::voxel_grid::world_coord> advancements;
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
            int diff;
            diff = compare(starting_coord.x, end_coord.x);
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
        auto test_hit = [&ray](tag::voxel_grid::world_coord coord){
            auto aabb = util::AABB(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5, 1, 1, 1);
            auto collision_info = util::findEnterExitFraction(ray, aabb);
            return collision_info.first;
        };

        std::vector<tag::voxel_grid::world_coord> coords_on_ray;

        auto current_coord = starting_coord;
        while(current_coord != end_coord){
            bool advanced = false;
            for(auto advancement : advancements){
                if(test_hit(current_coord + advancement)){
                    current_coord += advancement;
                    coords_on_ray.push_back(current_coord);
                    advanced = true;
                    break;
                }
            }
            if(!advanced) {
                std::cout << "'ray_to_block_coords(const util::Ray&)' needed to be bailed out!\n";
                break; // this just makes sure that *if* the logic is wrong, we don't get stuck looping for ever
            }
        }
        return coords_on_ray;
    }
}

namespace tag {
    game_world::game_world(registry_block_type block_registry): block_registry(block_registry){
    }

    void game_world::update(float dt){
    }

    int game_world::add_dimenion(voxel_grid::chunk_generator generator){
        dimensions.emplace_back(generator);
        return dimensions.size();
    }

    void game_world::display_chunks(unsigned int dimensionID, const voxel_grid::world_coord& coord_world, int range, const util::Camera& camera) const {
        if(dimensionID > dimensions.size()) {
            std::cout << "attempted to get display chunks for a dimension that doesn't exist\n";
            // return std::vector<voxel_grid::display_chunk>();
            return;
        }
        if(!block_registry) {
            std::cout << "We have no block_registry\n";
            return;
        }
        dimensions[dimensionID].display_chunks(block_registry, coord_world, range, camera);
    }

    void game_world::mine_blocks(unsigned int dimensionID, util::Ray& ray, int mining_power) {
        if(dimensionID > dimensions.size()) {
            std::cout << "attempted to mine blocks in a dimension that doesn't exist\n";
            return;
        }
        auto air_id = block_registry->get_id("core::air").value();

        auto block_coords = ray_to_block_coords(ray);
        for(auto& coord : block_coords){
            auto block_instance = dimensions[dimensionID].get_block(coord);
            if(block_instance.type_id != air_id) {
                dimensions[dimensionID].set_block({air_id, 0, 0, 0}, coord);
                break;
            }
        }
    }

    void game_world::place_block(unsigned int dimensionID, util::Ray& ray, int place_id) {
        if(dimensionID > dimensions.size()) {
            std::cout << "attempted to mine blocks in a dimension that doesn't exist\n";
            return;
        }
        auto block_coords = ray_to_block_coords(ray);

        for(unsigned int i = 0; i + 1 < block_coords.size(); i++){
            auto current_coord = block_coords[i];
            auto next_coord = block_coords[i+1];

            auto current_block_instance = dimensions[dimensionID].get_block(current_coord);
            auto next_block_instance = dimensions[dimensionID].get_block(next_coord);

            auto current_block_type = block_registry->get(current_block_instance.type_id).value();
            auto next_block_type = block_registry->get(next_block_instance.type_id).value();

            if(current_block_type.get_flag(block_type_flag::can_be_replaced) && !next_block_type.get_flag(block_type_flag::can_be_replaced)) {
                dimensions[dimensionID].set_block({place_id, 0, 0, 0}, current_coord);
                break;
            }
        }
    }

    bool game_world::is_passable(unsigned int dimensionID, const voxel_grid::world_coord& coord) const {
        if(dimensionID > dimensions.size()) {
            std::cout << "attempted to mine blocks in a dimension that doesn't exist\n";
            return false;
        }
        auto block_instance = dimensions[dimensionID].get_block(coord);
        auto block_type = block_registry->get(block_instance.type_id).value();
        return block_type.get_flag(block_type_flag::passable);
    }
}