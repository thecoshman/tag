#pragma once

#include "tag/block_type.hpp"
#include "util/registry.hpp"

namespace tag {
    class game_world {
        util::registry<tag::block_type> block_registry;

        public:
        game_world();
    };
}