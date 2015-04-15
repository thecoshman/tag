#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // for le translate
#include "voxel_grid/coord.hpp"

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
}