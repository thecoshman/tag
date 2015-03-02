#include <glm/glm.hpp>

namespace util{
struct AABB{
    AABB(glm::vec3 midPoint, double width, double height, double depth): AABB(midPoint.x, midPoint.y, midPoint.z, width, height, depth) {}
    AABB(double x, double y, double z, double width, double height, double depth):
        min(x - (width / 2), y - (height / 2), z - (depth / 2)),
        max(x + (width / 2), y + (height / 2), z + (depth / 2)) {}

    glm::vec3 min;
    glm::vec3 max;
};
}