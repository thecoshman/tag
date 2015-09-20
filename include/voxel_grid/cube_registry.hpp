#pragma once
#include <vector>
#include <stdexcept>

#include "cube_type.hpp"
//#include <string>

namespace voxel_grid{
// inline cube_flags operator|(cube_flags a, cube_flags b){
//     return static_cast<cube_flags>(static_cast<int>(a) | static_cast<int>(b));
// }
// inline cube_flags operator&(cube_flags a, cube_flags b){
//     return static_cast<cube_flags>(static_cast<int>(a) & static_cast<int>(b));
// }
// inline cube_flags operator^(cube_flags a, cube_flags b){
//     return static_cast<cube_flags>(static_cast<int>(a) ^ static_cast<int>(b));
// }
// inline cube_flags& operator|=(cube_flags& a, cube_flags b){
//     return static_cast<cube_flags&>(static_cast<int&>(a) |= static_cast<int>(b));
// }
// inline cube_flags& operator&=(cube_flags& a, cube_flags b){
//     return static_cast<cube_flags&>(static_cast<int&>(a) &= static_cast<int>(b));
// }
// inline cube_flags& operator^=(cube_flags& a, cube_flags b){
//     return static_cast<cube_flags&>(static_cast<int&>(a) ^= static_cast<int>(b));
// }

// template<class T> inline T operator~ (T a) { return (T)~(int)a; }
// template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
// template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
// template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
// template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
// template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
// template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

using cube_type_id = int;

// class cube_template{
//     cube_flags flags;
//     std::string name;
// public:
//     bool is_flag_set(cube_flags test_for) const{
//         return (flags & test_for) == test_for
//     }
// };

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