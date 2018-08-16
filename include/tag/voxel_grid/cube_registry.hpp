#pragma once
#include <vector>
#include <stdexcept>

#include "cube_type.hpp"

namespace tag {
    namespace voxel_grid{
        using cube_type_id = int;

        struct cube_instance{
        };

        struct cube_type_registry{
            cube_type_id register_new_type(cube_type new_cube_type){
                cube_types.push_back(new_cube_type);
                return cube_types.size() - 1;
            };

            const cube_type& get(cube_type_id id) const{
                try{
                    return cube_types.at(id);
                }
                catch (const std::out_of_range &e){
                    std::cout << "attempted to read cube_type_id=" << id << " out of " << cube_types.size() << "\n";
                    throw e;
                }
            }
        private:
            std::vector<cube_type> cube_types;
        };
    }
}
