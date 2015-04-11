#pragma once
#include <glm/glm.hpp>

namespace voxel_grid{
    const int chunk_size = 16;

    struct world_tag;
    struct chunk_tag;
    struct intra_chunk_tag;

    template <typename tag>
    struct coord{
        int x, y, z;

        coord(): x(0), y(0), z(0){}
        coord(int x, int y, int z): x(x), y(y), z(z){}

        static coord<tag> fromGlmVec3(const glm::vec3& realPosition){
            int x = static_cast<int>(std::floor(realPosition.x));
            int y = static_cast<int>(std::ceil(realPosition.y));
            int z = static_cast<int>(std::floor(realPosition.z));
            return coord<tag>(x, y, z);
        }

        bool operator<(const coord& rhs) const{
            return std::tie(x,y,z) < std::tie(rhs.x, rhs.y, rhs.z);
        }

        inline bool operator==(const coord& rhs){
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        inline bool operator!=(const coord& rhs){
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

    using world_coord = coord<world_tag>;
    using chunk_coord = coord<chunk_tag>;
    using intra_chunk_coord = coord<intra_chunk_tag>;

    template <typename tag>
    std::ostream& operator<<(std::ostream& os, const coord<tag>& coord){
      os << "(" << coord.x << ", " << coord.y << ", " << coord.z << ")";
      return os;
    }

    world_coord to_world_coord(const chunk_coord& chunk){
        return {
            chunk.x * chunk_size,
            chunk.y * chunk_size,
            chunk.z * chunk_size
        };
    }

    world_coord to_world_coord(const chunk_coord& chunk, const intra_chunk_coord& intra_chunk){
        return {
            (chunk.x * chunk_size) + intra_chunk.x,
            (chunk.y * chunk_size) + intra_chunk.y,
            (chunk.z * chunk_size) + intra_chunk.z
        };
    }

    std::pair<chunk_coord, intra_chunk_coord> from_world_coord(const world_coord& coord){
        auto to_chunk = [](const int v){
            if(v < 0){
                return (v + 1) / chunk_size - 1;
            } else {
                return v / chunk_size;
            }
        };
        auto to_intra = [](const int v){
            if(v < 0){
                return (v % chunk_size + chunk_size) % chunk_size;
            } else {
                return v % chunk_size;
            }
        };
        chunk_coord chunk{
            to_chunk(coord.x),
            to_chunk(coord.y),
            to_chunk(coord.z)
        };
        intra_chunk_coord intra{
            to_intra(coord.x),
            to_intra(coord.y),
            to_intra(coord.z)
        };
        return std::make_pair(chunk, intra);
    }
}