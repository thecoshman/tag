#include "tag/voxel_grid/chunked_voxel_grid.hpp"

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

namespace tag {
    namespace voxel_grid {
        const block_instance& chunked_voxel_grid::get_block(const world_coord& coord_world) const {
            auto chunk_coord_pair = from_world_coord(coord_world);
            auto chunk = get_data_chunk(chunk_coord_pair.first);
            return chunk.get_block(chunk_coord_pair.second);
        }

        block_instance& chunked_voxel_grid::get_block(const world_coord& coord_world){
            auto chunk_coord_pair = from_world_coord(coord_world);
            auto chunk = get_data_chunk(chunk_coord_pair.first);
            return chunk.get_block(chunk_coord_pair.second);
        }

        void chunked_voxel_grid::set_block(block_instance block, const world_coord& coord_world){
            auto chunk_coord_pair = from_world_coord(coord_world);
            auto& chunk = get_data_chunk(chunk_coord_pair.first);
            chunk.set_block(block, chunk_coord_pair.second);
            display_chunk_cache.erase(chunk_coord_pair.first); // it's no longer valid
        }

        std::vector<display_chunk> chunked_voxel_grid::get_display_chunks(std::shared_ptr<util::registry<block_type>> block_registry, const world_coord& coord_world, int range) const{
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
                    if(!block_registry) {
                        std::cout << "Where's the block_registry gone?\n";
                    }
                    display_chunk fresh_display_chunk(block_registry, coord, chunk);
                    chunks_in_range.push_back(fresh_display_chunk);
                    display_chunk_cache.insert({coord, fresh_display_chunk});
                }
            }
            return chunks_in_range;
        }

        data_chunk& chunked_voxel_grid::get_data_chunk(const chunk_coord& coord) const{
            auto chunk_search = chunk_data.find(coord);
            if(chunk_search == chunk_data.end()){
                auto generated_chunk = generator.generate(coord);
                chunk_data.insert({coord, generated_chunk});
            }
            try{
                return chunk_data.at(coord);
            }
            catch (const std::exception& e){
                // std::cout << "oh noes, " << coord << "\n";
                std::cout << e.what() << "\n";
                throw e;
            }
        }
    }
}
