#include "tag/game_world.hpp"

namespace tag {
    game_world::game_world() {
        for(auto&& name : {
            "core::air",
            "core::stone",
            "core::dirt",
            "core::grass"}) {
            block_registry.register_name(name);
        }
        {
            auto type = tag::block_type{"core", "air"};
            type.set_flag(block_type_flag::can_be_replaced);
            block_registry.set("core::air", type);
        }
        {
            auto type = tag::block_type{"core", "stone"};
            type.set_flag(block_type_flag::can_be_replaced);
            type.render_type = tag::basic_cube_render_type{"white_cube"};
            block_registry.set("core::stone", type);
        }
        {
            auto type = tag::block_type{"core", "dirt"};
            type.set_flag(block_type_flag::can_be_replaced);
            type.render_type = tag::basic_cube_render_type{"red_cube"};
            block_registry.set("core::dirt", type);
        }
        {
            auto type = tag::block_type{"core", "grass"};
            type.set_flag(block_type_flag::can_be_replaced);
            type.render_type = tag::basic_cube_render_type{"green_cube"};
            block_registry.set("core::grass", type);
        }
    }
}