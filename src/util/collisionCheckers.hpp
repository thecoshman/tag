#pragma once

#include <iostream>

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

namespace{
enum ClipPlane : int{
    x, y, z
};

template<typename V>
std::pair<bool, std::pair<typename V::value_type, typename V::value_type>> clip(ClipPlane plane, const RAY<V>& ray, const AxisAlignedBoundingBox<V>& box, typename V::value_type nearest, typename V::value_type furthest){
    using T = typename V::value_type;

    T near, far;

    near  = (box.min[plane] - ray.source[plane]) / ray.direction[plane];
    far   = (box.max[plane] - ray.source[plane]) / ray.direction[plane];
    
    if(far < near){
        std::swap(near, far);
    }

    if(far < nearest || near > furthest){
        return std::make_pair(false, std::make_pair(-1,-1));
    }

    near = std::max(near, nearest);
    far  = std::min(far,  furthest);

    if(near > far){
        return std::make_pair(false, std::make_pair(-1,-1));
    }

    return std::make_pair(true, std::make_pair(near, far));
}    
}

template<typename V>
std::pair<bool, std::pair<typename V::value_type, typename V::value_type>> findEnterExitFraction(const RAY<V>& ray, const AxisAlignedBoundingBox<V>& box){
    using T = typename V::value_type;

    // the fracion of the line that is in the box
    T near = 0;
    T far  = 1;

    auto clipingInfo = clip(ClipPlane::x, ray, box, near, far);
    if(!clipingInfo.first){
        return std::make_pair(false, std::make_pair(-1,-1));
    }

    near = clipingInfo.second.first;
    far = clipingInfo.second.second;

    clipingInfo = clip(ClipPlane::y, ray, box, near, far);
    if(!clipingInfo.first){
        return std::make_pair(false, std::make_pair(-1,-1));
    }

    near = clipingInfo.second.first;
    far = clipingInfo.second.second;

    clipingInfo = clip(ClipPlane::z, ray, box, near, far);
    if(!clipingInfo.first){
        return std::make_pair(false, std::make_pair(-1,-1));
    }

    return std::make_pair(true, std::make_pair(near, far));
}


}
