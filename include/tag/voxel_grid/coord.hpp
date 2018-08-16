#pragma once

#include <iostream>
#include <tuple>

#include <glm/glm.hpp>

#include "tag/voxel_grid/voxel_grid_constants.hpp"

namespace tag {
    namespace voxel_grid{
        template <typename T>
        struct coord{
            int x, y, z;

            coord(): x(0), y(0), z(0){}
            coord(int x, int y, int z): x(x), y(y), z(z){}

            static coord<T> fromGlmVec3(const glm::vec3& realPosition){
                return coord<T>{
                    static_cast<int>(std::floor(realPosition.x)),
                    static_cast<int>(std::ceil(realPosition.y)),
                    static_cast<int>(std::floor(realPosition.z))
                };
            }

            bool operator>=(const coord& rhs) const {
                return std::tie(x, y, z) >= std::tie(rhs.x, rhs.y, rhs.z);
            }

            bool operator>(const coord& rhs) const {

                return std::tie(x, y, z) > std::tie(rhs.x, rhs.y, rhs.z);
            }

            bool operator<=(const coord& rhs) const {
                return std::tie(x, y, z) <= std::tie(rhs.x, rhs.y, rhs.z);
            }

            bool operator<(const coord& rhs) const {

                return std::tie(x, y, z) < std::tie(rhs.x, rhs.y, rhs.z);
            }

            inline bool operator==(const coord& rhs) const {
                return std::tie(x, y, z) == std::tie(rhs.x, rhs.y, rhs.z);
            }

            inline bool operator!=(const coord& rhs) const {
                return !(*this == rhs);
            }

            coord& operator+=(const coord& rhs){
                x += rhs.x;
                y += rhs.y;
                z += rhs.z;
                return *this;
            }

            friend coord operator+(coord lhs, const coord& rhs){
                return lhs += rhs;
            }

            coord& operator-=(const coord& rhs){
                x -= rhs.x;
                y -= rhs.y;
                z -= rhs.z;
                return *this;
            }

            friend coord operator-(coord lhs, const coord& rhs){
                return lhs -= rhs;
            }
        };

        struct world_tag;
        struct chunk_tag;
        struct intra_chunk_tag;
        using world_coord = coord<world_tag>;
        using chunk_coord = coord<chunk_tag>;
        using intra_chunk_coord = coord<intra_chunk_tag>;

        template <typename T>
        std::ostream& operator<<(std::ostream& os, const coord<T>& coord);

        world_coord to_world_coord(const chunk_coord& chunk);

        world_coord to_world_coord(const chunk_coord& chunk, const intra_chunk_coord& intra_chunk);

        std::pair<chunk_coord, intra_chunk_coord> from_world_coord(const world_coord& coord);
    }
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const tag::voxel_grid::coord<T>& coord) {
    os << "(" << coord.x << "," << coord.y << "," << coord.z << ")";
    return os;
}