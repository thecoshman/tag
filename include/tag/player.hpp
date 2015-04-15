#pragma once
//#include <glm/glm.hpp>
#include <cmath>

namespace tag{
struct player{
    enum direction{
        forward, backward, left, right,
    };

    void move(direction d){
        auto move_speed = walk_speed;
        if(!grounded){
            move_speed /= 10;
        }
        glm::vec3 desrired_velocity;
        switch (d){
            case direction::forward:
                desrired_velocity += forward_vector() * move_speed;break;
            case direction::backward:
                desrired_velocity += forward_vector() * -move_speed;break;
            case direction::left:
                desrired_velocity += right_vector() * -move_speed; break;
            case direction::right:
                desrired_velocity += right_vector() * move_speed; break;
        }
        if(desrired_velocity.x > 0.0f){
            velocity.x = std::max(velocity.x, desrired_velocity.x);
        } else {
            velocity.x = std::min(velocity.x, desrired_velocity.x);
        }
        if(desrired_velocity.z > 0.0f){
            velocity.z = std::max(velocity.z, desrired_velocity.z);
        } else {
            velocity.z = std::min(velocity.z, desrired_velocity.z);
        }
    }

    void jump(){
        if(grounded){
            grounded = false;
            velocity.y = jump_start_speed;
        }
    }

    void update(float dt){
        do_falling(dt);
        do_walking(dt);
    }

    glm::vec3 eye_point(){
        return glm::vec3(position.x, position.y + eye_height, position.z);
    }

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
    glm::vec3 forward_vector() const{
        return glm::normalize(glm::vec3(view_vector.x, 0.0, view_vector.z));
    }

    glm::vec3 right_vector() const{
        return glm::cross(view_vector, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void do_walking(float dt){
        auto proposed_position = position + (velocity * (dt/1000));
        proposed_position.y = position.y;

        auto aabb = util::AABB(proposed_position.x, proposed_position.y + (1.72/2), position.z, 0.8, 1.72, 0.8);
        if(is_space_free_query(aabb)){
            position.x = proposed_position.x;
        } else {
            velocity.x = 0.0f;
        }

        aabb = util::AABB(position.x, proposed_position.y + (1.72/2), proposed_position.z, 0.8, 1.72, 0.8);
        if(is_space_free_query(aabb)){
            position.z = proposed_position.z;
        } else {
            velocity.z = 0.0f;
        }

        float friction = 0.25;
        if(!grounded){
            friction = 0.01;
        }    
        auto apply_friction = [friction](float& vel){
            float speed_loss = vel * friction;
            if(abs(speed_loss) > abs(vel)){
                vel = 0.0f;
            } else {
                vel -= speed_loss;
            }
        };
        apply_friction(velocity.x);
        apply_friction(velocity.z);
    }

    void do_falling(float dt){
        if(grounded){
            auto aabb = util::AABB(position.x, position.y + (1.72/2) - 0.1, position.z, 0.8, 1.72, 0.8);
            if(is_space_free_query(aabb)){
                grounded = false;
            }
        } else {
            auto proposed_position = position;
            proposed_position.y += velocity.y * (dt/1000);
            velocity.y -= gravity * (dt/1000);

            auto aabb = util::AABB(proposed_position.x, proposed_position.y + (1.72/2), proposed_position.z, 0.8, 1.72, 0.8);
            if(proposed_position.y >= 0.0f && is_space_free_query(aabb)){
                position = proposed_position;
            } else {
                grounded = true;
                velocity.y = 0.0f;
            }
        }
    }
};
}