#include "util/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>

namespace util {
    Camera::Camera(): 
        pos(0.0,0.0,0.0),
        dir(0.0,0.0,-1.0),
        up(0.0,1.0,0.0),
        aspectRatio(4.0f / 3.0f),
        fov(45.0f), near(0.1f), far(999.0f) {}

    glm::mat4 Camera::projectionMatrix() const {
        return glm::perspective(fov, aspectRatio, near, far); 
    }

    void Camera::translate(const glm::vec3& translation) {
        pos += translation;
    }

    glm::vec3 Camera::rightVector() const {
        glm::normalize(dir);
        glm::normalize(up);
        return glm::cross(dir, up);
    }

    void Camera::rotateYaw(double degrees) {
        dir = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), -static_cast<float>(degrees), up) * glm::vec4(dir, 0.0f));
    }

    void Camera::rotatePitch(double degrees) {
        auto right = rightVector();
        // up = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), degrees, right) * glm::vec4(dir, 0.0f));
        dir = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), static_cast<float>(degrees), right) * glm::vec4(dir, 0.0f));
    }

    void Camera::rotateRoll(double degrees) {
        up = glm::swizzle<glm::X, glm::Y, glm::Z>(glm::rotate(glm::mat4(1.0f), static_cast<float>(degrees), dir) * glm::vec4(dir, 0.0f));
    }

    glm::mat4 Camera::viewMatrix() const {
        return glm::lookAt(pos, pos + dir, up);
    }

    glm::mat4 Camera::mvpMatrix(const glm::mat4 modelMatrix) const {
        return projectionMatrix() * viewMatrix() * modelMatrix;
    }

    void Camera::forward(float distance) {
        glm::normalize(dir);
        pos += dir * distance;
    }

    void Camera::strafe(float distance) {
        auto right = rightVector();
        pos += right * distance;
    }

    void Camera::pan(const glm::vec2& panning) {
        auto right = rightVector();
        pos += right * -panning.x;
        pos += up * panning.y;
    }
}