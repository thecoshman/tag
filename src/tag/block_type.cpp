#include "tag/block_type.hpp"

#include <iostream>

#include "tag/game_world.hpp"

namespace {
    bool default_on_break(tag::game_world gw) {
        std::cout << "Default 'on_break'\n";
        return true;
    }
}

namespace tag {
    block_type::block_type(const std::string& mod_name, const std::string& name):
        mod_name(mod_name), name(name),
        val_a(0), val_b(0), flags(0),
        on_break(default_on_break),
        render_type(tag::null_render_type{}) {
    }
    
    void block_type::set_flag(block_type_flag flag, bool value) {
        if(value){
            flags |= flag;
        } else {
            flags &= !flag;
        }
    }
    
    bool block_type::get_flag(block_type_flag flag) const {
        return flag == (flags & flag);
    }
}