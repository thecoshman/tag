#pragma once

#include <functional>
#include <string>
#include <variant>

namespace tag {
    struct game_world;

    enum block_type_flag : unsigned int {
        can_be_replaced = (1 <<  0),
        falls           = (1 <<  1),
        is_solid_block  = (1 <<  2)
    };

    struct null_render_type{
    };

    struct basic_cube_render_type{
        std::string texture_name;
    };

    struct block_type {
        std::string mod_name, name;
        int val_a, val_b;
        private:
        unsigned int flags;
        public:
        std::function<void(tag::game_world)> on_break;
        
        block_type(const std::string& mod_name, const std::string& name);

        ~block_type() = default;
        block_type(const block_type& other) = default;
        block_type(block_type&& other) noexcept = default;
        block_type& operator=(const block_type& other) = default;
        block_type& operator=(block_type&& other) noexcept = default;
        
        void set_flag(block_type_flag flag, bool value = true);
        
        bool get_flag(block_type_flag flag) const;

        std::variant<tag::null_render_type, tag::basic_cube_render_type> render_type;
    };
}