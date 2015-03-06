#pragma once

#include "volumes.hpp"

namespace util{
bool checkCollision(const AABB& first, const AABB& second){
    if(first.max.x < second.min.x){ return false; }
    if(first.max.y < second.min.y){ return false; }
    if(first.max.z < second.min.z){ return false; }

    if(first.min.x > second.max.x){ return false; }
    if(first.min.y > second.max.y){ return false; }
    if(first.min.z > second.max.z){ return false; }

    return true;
}

bool checkCollision(const AABB& box, const glm::vec3& point){
    if(box.max.x < point.x){ return false; }
    if(box.max.y < point.y){ return false; }
    if(box.max.z < point.z){ return false; }

    if(box.min.x > point.x){ return false; }
    if(box.min.y > point.y){ return false; }
    if(box.min.z > point.z){ return false; }

    return true;
}
}
