#include "tag/voxel_grid/display_chunk.hpp"

#include "tag/block_type.hpp"
#include "tag/voxel_grid/chunked_voxel_grid.hpp"
#include <type_traits>
#include <variant>
#include <optional>

#include <chrono>

namespace {
    template <typename T>
    void append(std::vector<T>& left, const std::vector<T>& right) {
        left.reserve(left.size() + right.size());
        left.insert(left.end(), right.begin(), right.end());
    }

    template<typename R>
    bool is_ready(std::future<R> const& f) {
        std::cout << "Attempting to get status of future for display chunk\n";
        return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }
}

namespace tag {
    namespace voxel_grid {
        display_chunk::display_chunk(std::shared_ptr<util::registry<block_type>> block_registry, const chunk_coord& coord_chunk, const data_chunk& chunk){
            generationFuture = std::async(std::launch::async, generate, block_registry, coord_chunk, chunk);
            // auto data = generate(block_registry, coord_chunk, chunk);
        }

        void display_chunk::display() {
            std::cout << "Going to try draw chunk\n";
            if (future_completed) {
                std::cout << "display chunk is good to be drawn\n";
                meshVAO.bind();
                gl::DrawElements(gl::TRIANGLES, 6 * 2 * 3 * cubeCount, gl::UNSIGNED_INT, 0);
            // } else if (is_ready(generationFuture) {
            } else if (generationFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                std::cout << "Future display chunk is ready\n";
                auto data = generationFuture.get();
                buffer_generated_data(data);
                future_completed = true;
            } else {
                std::cout << "Future display chunk is not ready\n";
            }
        }

        std::tuple<int, std::vector<GLuint>,std::vector<GLfloat>,std::vector<GLfloat>> display_chunk::generate(std::shared_ptr<util::registry<block_type>> block_registry, const chunk_coord& coord_chunk, const data_chunk& chunk){
            std::vector<GLuint> indexdata;
            std::vector<GLfloat> vertexPositions;
            std::vector<GLfloat> textureCoord;
            int generatedCubeCount = 0;

            unsigned int verticesPerCube = 24;

            for(int x = 0; x < chunk_size; ++x){
                for(int y = 0; y < chunk_size; ++y){
                    for(int z = 0; z < chunk_size; ++z){
                        // std::cout << "Display chunk building block at (" << x << ", " << y << ", " << z << ")\n";
                        auto coord_intra_chunk = intra_chunk_coord{x, y, z};
                        if(needs_rendering(block_registry, chunk, coord_intra_chunk)){
                            append(indexdata, generateCubeIndexNumbers(generatedCubeCount * verticesPerCube));
                            append(vertexPositions, generateCubeVertexPositions(coord_intra_chunk));
                            append(textureCoord, generateCubeTextureCoords(block_registry, chunk.get_block(coord_intra_chunk).type_id));
                            generatedCubeCount++;
                        }
                    }
                }
            }
            std::cout << "display chunk finished building data\n";
            return std::make_tuple(generatedCubeCount, indexdata, vertexPositions, textureCoord);
        }

        void display_chunk::buffer_generated_data(std::tuple<int, std::vector<GLuint>,std::vector<GLfloat>,std::vector<GLfloat>> data) {
            std::cout << "Buffering data into ogl\n";
            auto [ generatedCubeCount, indexdata, vertexPositions, textureCoord ] = data;
            
            cubeCount = generatedCubeCount;

            meshVAO.bind();

            indexBuffer.bufferData(indexdata);

            vertexBuffer.bufferData(vertexPositions);
            gl::EnableVertexAttribArray(0);
            gl::VertexAttribPointer(0, 3, gl::FLOAT, gl::FALSE, 0, 0);
            textureCoordBuffer.bufferData(textureCoord);
            gl::EnableVertexAttribArray(1);
            gl::VertexAttribPointer(1, 2, gl::FLOAT, gl::FALSE, 0, 0);
            std::cout << "Finshed buffer into ogl\n";
        }

        bool display_chunk::needs_rendering(std::shared_ptr<util::registry<block_type>> block_registry, const data_chunk& chunk, const intra_chunk_coord& coord_intra_chunk){
            auto block = chunk.get_block(coord_intra_chunk);
            auto type_of_block = block_registry->get(block.type_id).value();

            if(type_of_block.get_flag(block_type_flag::invisible)){
                return false;
            }
            if(at_chunk_edge(coord_intra_chunk)){
                return true;
            }
            if(all_neighburs_block_los(block_registry, chunk, coord_intra_chunk)){
                return false;
            }
            return true;
        }

        bool display_chunk::at_chunk_edge(const intra_chunk_coord& coord_intra_chunk){
            if(coord_intra_chunk.x == 0 || coord_intra_chunk.x == (chunk_size - 1)){ return true; }
            if(coord_intra_chunk.y == 0 || coord_intra_chunk.y == (chunk_size - 1)){ return true; }
            if(coord_intra_chunk.z == 0 || coord_intra_chunk.z == (chunk_size - 1)){ return true; }
            return false;
        }
        
        bool display_chunk::all_neighburs_block_los(std::shared_ptr<util::registry<block_type>> block_registry, const data_chunk& chunk, const intra_chunk_coord& coord){
            auto blocks_los = [block_registry, chunk, coord](intra_chunk_coord offset){
                auto test_coord = coord + offset;
                auto block = chunk.get_block(test_coord);
                auto block_type = block_registry->get(block.type_id).value();
                return block_type.get_flag(block_type_flag::fully_blocks_los);       
            };

            return 
                blocks_los({-1, 0, 0}) &&
                blocks_los({ 1, 0, 0}) &&
                blocks_los({ 0,-1, 0}) &&
                blocks_los({ 0, 1, 0}) &&
                blocks_los({ 0, 0,-1}) &&
                blocks_los({ 0, 0, 1});
        }

        std::vector<GLuint> display_chunk::generateCubeIndexNumbers(unsigned int offset) {
            return {
                // -x face
                offset + 0,  offset + 1,  offset + 2,
                offset + 2,  offset + 3,  offset + 0,
                // // +z face
                offset + 4,  offset + 5,  offset + 6,
                offset + 6,  offset + 7,  offset + 4,
                // +x face
                offset + 8,  offset + 9,  offset + 10,
                offset + 10, offset + 11, offset + 8,
                // -z face
                offset + 12, offset + 13, offset + 14,
                offset + 14, offset + 15, offset + 12,
                // +y face
                offset + 16, offset + 17, offset + 18,
                offset + 18, offset + 19, offset + 16,
                // -y face
                offset + 20, offset + 21, offset + 22,
                offset + 22, offset + 23, offset + 20,
            };
        }

        std::vector<GLfloat> display_chunk::generateCubeVertexPositions(const intra_chunk_coord& coord_intra_chunk) {
            auto x = static_cast<GLfloat>(coord_intra_chunk.x);
            auto y = static_cast<GLfloat>(coord_intra_chunk.y);
            auto z = static_cast<GLfloat>(coord_intra_chunk.z);

            return {
                // 0, 1, 2, 3
                x       , y       , z       , 
                x       , y       , z + 1.0f, 
                x       , y + 1.0f, z + 1.0f, 
                x       , y + 1.0f, z       , 
                // 4, 5, 6, 7
                x       , y       , z + 1.0f, 
                x + 1.0f, y       , z + 1.0f, 
                x + 1.0f, y + 1.0f, z + 1.0f, 
                x       , y + 1.0f, z + 1.0f, 
                // 8, 9,10,11
                x + 1.0f, y       , z + 1.0f, 
                x + 1.0f, y       , z       , 
                x + 1.0f, y + 1.0f, z       , 
                x + 1.0f, y + 1.0f, z + 1.0f, 
                // 12,13,14,15
                x + 1.0f, y       , z       , 
                x       , y       , z       , 
                x       , y + 1.0f, z       , 
                x + 1.0f, y + 1.0f, z       , 
                // 16,17,18,19
                x       , y + 1.0f, z       , 
                x       , y + 1.0f, z + 1.0f, 
                x + 1.0f, y + 1.0f, z + 1.0f, 
                x + 1.0f, y + 1.0f, z       , 
                // 20,21,22,23
                x       , y       , z + 1.0f, 
                x       , y       , z       , 
                x + 1.0f, y       , z       , 
                x + 1.0f, y       , z + 1.0f, 
            };
        }

        std::vector<GLfloat> display_chunk::generateCubeTextureCoords(std::shared_ptr<util::registry<block_type>> block_registry, int type_id) {
            auto type = block_registry->get(type_id).value();
            auto rendering_model = type.renderingModel;

            return std::visit([](auto&& arg) -> std::vector<GLfloat> {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, block_rendering::SixSided>) {
                    return {
                        arg.textureCoords.fromU, arg.textureCoords.fromV,
                        arg.textureCoords.fromU, arg.textureCoords.toV,  
                        arg.textureCoords.toU,   arg.textureCoords.toV,  
                        arg.textureCoords.toU,   arg.textureCoords.fromV,

                        arg.textureCoords.fromU, arg.textureCoords.fromV,
                        arg.textureCoords.toU,   arg.textureCoords.fromV,
                        arg.textureCoords.toU,   arg.textureCoords.toV,  
                        arg.textureCoords.fromU, arg.textureCoords.toV,  

                        arg.textureCoords.fromU, arg.textureCoords.toV,  
                        arg.textureCoords.fromU, arg.textureCoords.fromV,
                        arg.textureCoords.toU,   arg.textureCoords.fromV,
                        arg.textureCoords.toU,   arg.textureCoords.toV,  

                        arg.textureCoords.toU,   arg.textureCoords.fromV,
                        arg.textureCoords.fromU, arg.textureCoords.fromV,
                        arg.textureCoords.fromU, arg.textureCoords.toV,  
                        arg.textureCoords.toU,   arg.textureCoords.toV,  

                        arg.textureCoords.fromU, arg.textureCoords.fromV,
                        arg.textureCoords.fromU, arg.textureCoords.toV,  
                        arg.textureCoords.toU,   arg.textureCoords.toV,  
                        arg.textureCoords.toU,   arg.textureCoords.fromV,

                        arg.textureCoords.toU,   arg.textureCoords.toV,  
                        arg.textureCoords.toU,   arg.textureCoords.fromV,
                        arg.textureCoords.fromU, arg.textureCoords.fromV,
                        arg.textureCoords.fromU, arg.textureCoords.toV,  
                    };
                } else {
                    return {

                    };
                }
                }, rendering_model
            );
        }
    }
}