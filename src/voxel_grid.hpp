#pragma once

struct CubeCoord{
    int x, y, z;

    CubeCoord(int x, int y, int z): x(x), y(y), z(z){}

    static CubeCoord fromGlmVec3(const glm::vec3& realPosition){
        int x = static_cast<int>(std::floor(realPosition.x));
        int y = static_cast<int>(std::ceil(realPosition.y));
        int z = static_cast<int>(std::floor(realPosition.z));
        return CubeCoord{x, y, z};
    }

    bool operator<(const CubeCoord& rhs) const{
        return std::tie(x,y,z) < std::tie(rhs.x, rhs.y, rhs.z);
    }
};

struct Cube{
    static glm::mat4 getModelMatrix(CubeCoord coord){
        return glm::translate(glm::mat4(1.0f), glm::vec3(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5));
    }
    
    Cube(std::string textureName): textureName(textureName){};
    Cube(std::string textureName, bool can_move_through): textureName(textureName), is_passable(can_move_through){};

    std::string textureName;
    bool is_passable = false;
};

template<typename V, typename T = typename V::value_type>
std::pair<std::map<CubeCoord, Cube>::const_iterator, std::pair<T, T>> findClosestHit(const util::RAY<V>& ray, const std::map<CubeCoord, Cube>& worldGrid){
    // auto starting_coord = CubeCoord::fromGlmVec3(ray.source);

    T max_t = std::numeric_limits<T>::max();
    std::pair<T, T> hit_info_for_closest{max_t, max_t};
    auto nearest = worldGrid.end();

    for(auto ittr = worldGrid.begin(); ittr != worldGrid.end(); ittr++){
        auto& coord = ittr->first;
        auto aabb = util::AABB(coord.x + 0.5, coord.y - 0.5, coord.z + 0.5, 1, 1, 1);

        auto collision_info = util::findEnterExitFraction(ray, aabb);
        if(collision_info.first){
            bool new_closest = (nearest == worldGrid.end()) || (collision_info.second.first < hit_info_for_closest.first);
            if(new_closest){
                nearest = ittr;
                hit_info_for_closest = collision_info.second;
            }
        }
    }
    return std::make_pair(nearest, hit_info_for_closest);
}

struct voxel_grid{
    void generate_world(){
        grid.insert({{ 0,  2,  0}, red_cube_template});
        grid.insert({{ 2,  1,  2}, red_cube_template});
        grid.insert({{ 10, 1, 10}, red_cube_template});
        grid.insert({{ 5,  1,  5}, red_cube_template});
        grid.insert({{ 5,  1,  5}, red_cube_template});
        grid.insert({{ 5,  1, -5}, green_cube_template});
        grid.insert({{ 3,  1, -5}, green_cube_template});
        grid.insert({{-3,  4, -5}, green_cube_template});

        for(int x = -100; x <= 100; x++){
            for(int z = -100; z <= 100; z++){
                // for(int y = 0; y < 10; y++){
                //     float value =  util::simplex_noise(1, x, y, z);
                //     if(value > 1.0){
                //         grid.insert({{ x,  y + 1,  z}, white_cube_template});
                //     }
                // }
                float size = 16;

                float xf = x / (size * 6.0);
                float zf = z / (size * 6.0);

                double value = util::simplex_noise(1, xf, zf);

                //compress the range a bit:
                value *= 0.7;
                value += 0.2;
                if (value > 1.) value = 1.;

                int h = value * size;

                for (int i = 0; i < h; ++i) {
                    if(i < 0.5*size){
                        grid.insert({{ x, i,  z}, white_cube_template});
                    }
                }
                grid.insert({{ x, 0,  z}, red_cube_template});
            }
        }
    }

    Cube get(const CubeCoord& coord){
        auto search = grid.find(coord);
        if(search == grid.end()) {
            return empty;
        }
        else {
            return search->second;
        }
    }

    std::map<CubeCoord, Cube> grid;

    const Cube empty = Cube("empty", true);
    const Cube red_cube_template = Cube("red_cube", false);
    const Cube white_cube_template = Cube("white_cube", false);
    const Cube green_cube_template = Cube("green_cube", false);
};