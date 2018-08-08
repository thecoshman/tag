#pragma once
#include <cmath>
#include <functional>

#include <glm/glm.hpp>

#include "util/volumes.hpp"

namespace tag{
    struct player{
        enum direction{
            forward, backward, left, right,
        };

        void move(direction d);

        void jump();

        void update(float dt);

        glm::vec3 eye_point();

        const float eye_height = 1.72;
        const float walk_speed = 8.0f;
        const float jump_start_speed = 4.9f;
        const float gravity = 9.8f;

        glm::vec3 view_vector;
        glm::vec3 position;
        glm::vec3 velocity;
        bool grounded = true;
        std::function<bool(util::AABB const&)> is_space_free_query = [](util::AABB const & aabb){ return true;};

        private:
        glm::vec3 forward_vector() const;

        glm::vec3 right_vector() const;

        void do_walking(float dt);

        void do_falling(float dt);
    };
}