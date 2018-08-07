#pragma once

#include <iostream>

#include "volumes.hpp"

namespace util{
namespace{
enum ClipPlane : int{
    x, y, z
};

template<typename V, typename T = typename V::value_type>
std::pair<bool, std::pair<T, T>> clip(ClipPlane plane, const RAY<V>& ray, const AxisAlignedBoundingBox<V>& box, const T& nearest, const T& furthest){
    T near = (box.min[plane] - ray.source[plane]) / ray.direction[plane];
    T far  = (box.max[plane] - ray.source[plane]) / ray.direction[plane];
    
    if(far < near){
        std::swap(near, far);
    }

    if(far < nearest || near > furthest){
        return std::make_pair(false, std::make_pair(-1,-1));
    }

    near = std::max(near, nearest);
    far  = std::min(far,  furthest);

    if(nearest > furthest){
        return std::make_pair(false, std::make_pair(-1,-1));
    }

    return std::make_pair(true, std::make_pair(near, far));
}    
}

template<typename V>
void orderDimension(int d, V& lhs, V& rhs){
    if(lhs[d] < rhs[d]){
        std::swap(lhs[d], rhs[d]);
    }
}

template<typename V, typename T = typename V::value_type>
std::pair<bool, std::pair<T, T>> findEnterExitFraction(const RAY<V>& ray, const AxisAlignedBoundingBox<V>& box){
    // the fracion of the line that is in the box
    T near = 0;
    T far  = 1;

    auto clipping_info = clip(ClipPlane::x, ray, box, near, far);
    if(!clipping_info.first){
        return std::make_pair(false, std::make_pair(-1,-1));
    }
    near = clipping_info.second.first;
    far  = clipping_info.second.second;

    clipping_info = clip(ClipPlane::y, ray, box, near, far);
    if(!clipping_info.first){
        return std::make_pair(false, std::make_pair(-1,-1));
    }
    near = clipping_info.second.first;
    far  = clipping_info.second.second;

    clipping_info = clip(ClipPlane::z, ray, box, near, far);
    if(!clipping_info.first){
        return std::make_pair(false, std::make_pair(-1,-1));
    }
    near = clipping_info.second.first;
    far  = clipping_info.second.second;

    return std::make_pair(true, std::make_pair(near, far));
}
}
