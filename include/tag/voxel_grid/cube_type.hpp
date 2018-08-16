#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // for le translate

#include "tag/voxel_grid/coord.hpp"

namespace tag {
    namespace voxel_grid {
        enum class cube_flags{
            none                = 1 << 0,
            invisible           = 1 << 1,
            gravity             = 1 << 2,
            passable            = 1 << 3,
            fully_blocks_los    = 1 << 4,
        };

        inline cube_flags operator|(cube_flags a, cube_flags b){
            return static_cast<cube_flags>(static_cast<int>(a) | static_cast<int>(b));
        }
        inline cube_flags operator&(cube_flags a, cube_flags b){
            return static_cast<cube_flags>(static_cast<int>(a) & static_cast<int>(b));
        }

        struct cube_type{
            static glm::mat4 getModelMatrix(world_coord coord){
                return glm::translate(glm::mat4(1.0f), glm::vec3(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5));
            }        

            cube_type(cube_flags flags, std::string name, std::string textureName): flags(flags), name(name), textureName(textureName){ };

            cube_flags flags;
            std::string name;
            std::string textureName;

            bool is_flag_set(const cube_flags& test_for) const{
                return (flags & test_for) == test_for;
            }
        };
    }
}
