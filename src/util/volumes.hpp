#pragma once

#include <glm/glm.hpp>

namespace util{

template <typename V>
struct AxisAlignedBoundingBox{
    using T = typename V::value_type;

    AxisAlignedBoundingBox(V midPoint, T width, T height, T depth): AxisAlignedBoundingBox(midPoint.x, midPoint.y, midPoint.z, width, height, depth) {}
    AxisAlignedBoundingBox(T x, T y, T z, T width, T height, T depth):
        min(x - (width / 2), y - (height / 2), z - (depth / 2)),
        max(x + (width / 2), y + (height / 2), z + (depth / 2)) {}

    V min, max;
};

typedef AxisAlignedBoundingBox<glm::vec3> AABB;

template <typename V>
struct RAY{
    V source, direction;
};
typedef RAY<glm::vec3> Ray;
}
