#include "tag/voxel_grid/chunked_voxel_grid.hpp"

#include <glm/gtc/matrix_transform.hpp>

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
        const std::optional<std::reference_wrapper<block_instance>> chunked_voxel_grid::get_block(const world_coord& coord_world) const {
            return get_block(coord_world);
        }

        std::optional<std::reference_wrapper<block_instance>> chunked_voxel_grid::get_block(const world_coord& coord_world){
            std::cout << "Get block\n";
            auto chunk_coord_pair = from_world_coord(coord_world);
            auto chunk = get_data_chunk(chunk_coord_pair.first);
            if (chunk) {
                return chunk->get().get_block(chunk_coord_pair.second);
            }
            std::cout << "Chunk must not be loaded yet\n";
            return std::nullopt;
        }

        void chunked_voxel_grid::set_block(block_instance block, const world_coord& coord_world){
            auto chunk_coord_pair = from_world_coord(coord_world);
            auto chunk = get_data_chunk(chunk_coord_pair.first);
            if (chunk) {
                chunk->get().set_block(block, chunk_coord_pair.second);
            } else {
                std::cout << "A block was placed in a chunk that wasn't loaded... what do :(\n";
            }
            display_chunk_cache.erase(chunk_coord_pair.first); // it's no longer valid
        }

        void chunked_voxel_grid::display_chunks(std::shared_ptr<util::registry<block_type>> block_registry, const world_coord& coord_world, int range, const util::Camera& camera) const{
            auto main_chunk_coord = from_world_coord(coord_world).first;
            auto chunk_range = make_coord_range<chunk_coord>(
                {main_chunk_coord.x - range, main_chunk_coord.y - range, main_chunk_coord.z - range},
                {main_chunk_coord.x + range, main_chunk_coord.y + range, main_chunk_coord.z + range});

            chunk_shader.use();
            GLint mvpMat = chunk_shader.getUniformLocation("mvpMat");
            auto projectViewMatrix = camera.projectionMatrix() * camera.viewMatrix();
           
            for(auto coord : chunk_range){
                auto modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(coord.x * chunk_size, (coord.y * chunk_size) - 1, coord.z * chunk_size));
                gl::UniformMatrix4fv(mvpMat, 1, gl::FALSE, glm::value_ptr(projectViewMatrix * modelMatrix));
                auto chunk = get_display_chunk(block_registry, coord);
                if(chunk) {
                    std::cout << "Got display chunk to try display\n";
                    chunk->get().display();
                    std::cout << "finshed 'displaying' chunk\n";
                }
            }
            std::cout << "finshed displaying all chunks\n";
        }

        std::optional<std::reference_wrapper<data_chunk>> chunked_voxel_grid::get_data_chunk(const chunk_coord& coord) const{

            auto chunk_search = chunk_data.find(coord);
            if(chunk_search != chunk_data.end()){
                return chunk_data.at(coord);
            } 

            auto future_chunk_search = future_chunk_data.find(coord);
            if (future_chunk_search != future_chunk_data.end()) {
                std::cout << "[SERVER] Chunk loading already triggered\n";
                // auto future_chunk = std::move(future_chunk_search->second);
                if (future_chunk_data.at(coord).wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    std::cout << "[SERVER] Finished loading of chunk (" << coord.x << "," << coord.y << "," << coord.z << ")\n";
                    // auto loaded_data_chunk = future_chunk.get();
                    auto loaded_data_chunk = future_chunk_data.at(coord).get();
                    chunk_data.insert({coord, loaded_data_chunk});
                    future_chunk_data.erase(future_chunk_search);
                    return chunk_data.at(coord);
                } else {
                    std::cout << "[SEVER] Chunk is not ready yet\n";
                    // future_chunk_data.insert({coord, std::move(future_chunk)});
                }
            } else {
                std::cout << "[SERVER] Starting async load of chunk (" << coord.x << "," << coord.y << "," << coord.z << ")\n";
                auto future_chunk = std::async(std::launch::async, &chunk_generator::generate, &generator, coord);
                future_chunk_data.insert({coord, std::move(future_chunk)});
            }
            return std::nullopt;
        }

        std::optional<std::reference_wrapper<display_chunk>> chunked_voxel_grid::get_display_chunk(std::shared_ptr<util::registry<block_type>> block_registry, const chunk_coord& coord) const{
            auto display_chunk_search = display_chunk_cache.find(coord);
            if(display_chunk_search != display_chunk_cache.end()){
                return display_chunk_cache.at(coord);
            }

            // not previously cached this disply chunk (or dropped it)
            auto data_chunk = get_data_chunk(coord);
            if (data_chunk){
                // data chunk is loaded so start trying to build it
                auto fresh_display_chunk = display_chunk(block_registry, coord, data_chunk.value());
                display_chunk_cache.insert({coord, std::move(fresh_display_chunk)});
                return display_chunk_cache.at(coord);
            } 
            return std::nullopt;
            // try{
            // }
            // catch (const std::exception& e){
            //     std::cout << "ohunked_voxel_grid::get_display_chunk\n";
            //     std::cout << e.what() << "\n";
            //     throw e;
            // }
        }
    }
}
