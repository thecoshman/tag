#pragma once

#include <glm/glm.hpp>

namespace util{
    struct Camera{
        Camera(): 
            pos(0.0,0.0,0.0),
            dir(0.0,0.0,-1.0),
            up(0.0,1.0,0.0),
            aspectRatio(4.0f / 3.0f),
            fov(45.0f), near(0.1f), far(999.0f){}

        glm::mat4 projectionMatrix() const{
            return glm::perspective(fov, aspectRatio, near, far); 
        }

        void translate(const glm::vec3& translation){
            pos += translation;
        }

        glm::vec3 rightVector() const {
            glm::normalize(dir);
            glm::normalize(up);
            return glm::cross(dir, up);
        }

        void rotateYaw(float degrees) {
            dir = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), -degrees, up) * glm::vec4(dir, 0.0f));
        }

        void rotatePitch(float degrees) {
            auto right = rightVector();
            // up = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), degrees, right) * glm::vec4(dir, 0.0f));
            dir = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), degrees, right) * glm::vec4(dir, 0.0f));
        }

        void rotateRoll(float degrees) {
            up = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), degrees, dir) * glm::vec4(dir, 0.0f));
        }

        glm::mat4 viewMatrix() const {
            return glm::lookAt(pos, pos + dir, up);
        }

        glm::mat4 mvpMatric(const glm::mat4 modelMatrix) const {
            return projectionMatrix() * viewMatrix() * modelMatrix;
        }

        void forward(float distance) {
            glm::normalize(dir);
            pos += dir * distance;
        }

        void strafe(float distance) {
            auto right = rightVector();
            pos += right * distance;
        }

        void pan(glm::vec2 panning){
            auto right = rightVector();
            pos += right * -panning.x;
            pos += up * panning.y;
        }

        glm::vec3 pos, dir, up;
        float aspectRatio, fov, near, far;
    };
}