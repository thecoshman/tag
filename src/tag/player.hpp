#pragma once
//#include <glm/glm.hpp>

namespace tag{
struct player{
    enum direction {
        forward, backward, left, right,
    };

    void move(direction d){
        glm::vec3 translation;
        switch (d){
            case direction::forward:
                translation = forward_vector() * walk_speed;break;
            case direction::backward:
                translation = forward_vector() * -walk_speed;break;
            case direction::left:
                translation = right_vector() * -walk_speed; break;
            case direction::right:
                translation = right_vector() * walk_speed; break;
        }
        auto proposed_position = position + translation;

        auto aabb = util::AABB(proposed_position.x, proposed_position.y + (1.72/2), proposed_position.z, 0.8, 1.72, 0.8);
        if(is_space_free_query(aabb)){
            position = proposed_position;
        }
    }

    void jump(){
        if(grounded){
            grounded = false;
            v_speed = jump_start_speed;
        }
    }

    void apply_gravity(){
        if(grounded){
            auto aabb = util::AABB(position.x, position.y + (1.72/2) - 0.1, position.z, 0.8, 1.72, 0.8);
            if(is_space_free_query(aabb)){
                grounded = false;
            }
        } else {
            auto proposed_position = position;
            proposed_position.y += v_speed;
            v_speed -= 0.04f;

            auto aabb = util::AABB(proposed_position.x, proposed_position.y + (1.72/2), proposed_position.z, 0.8, 1.72, 0.8);
            if(proposed_position.y >= 0.0f && is_space_free_query(aabb)){
                position = proposed_position;
            } else {
                grounded = true;
                v_speed = 0.0f;
            }
        }
    }

    glm::vec3 eye_point(){
        return glm::vec3(position.x, position.y + eye_height, position.z);
    }

    const float eye_height = 1.72;
    const float walk_speed = 0.1;
    const float jump_start_speed = 0.4f;

    glm::vec3 view_vector;
    glm::vec3 position;
    float v_speed = 0.0f;
    bool grounded = true;
    std::function<bool(util::AABB const&)> is_space_free_query = [](util::AABB const & aabb){ return true;};


    private:
    glm::vec3 forward_vector() const{
        return glm::normalize(glm::vec3(view_vector.x, 0.0, view_vector.z));
    }
    glm::vec3 right_vector() const{
        return glm::cross(view_vector, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};
}