#pragma once

#include <glm/glm.hpp>

namespace util{
    struct Camera{
        Camera();

        glm::mat4 projectionMatrix() const;

        void translate(const glm::vec3& translation);

        glm::vec3 rightVector() const ;

        void rotateYaw(double degrees);

        void rotatePitch(double degrees);

        void rotateRoll(double degrees);

        glm::mat4 viewMatrix() const;

        glm::mat4 mvpMatrix(const glm::mat4 modelMatrix) const;

        void forward(float distance);

        void strafe(float distance);

        void pan(const glm::vec2& panning);

        glm::vec3 pos, dir, up;
        float aspectRatio, fov, near, far;
    };
}