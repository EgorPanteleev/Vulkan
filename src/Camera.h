//
// Created by auser on 5/4/25.
//

#ifndef VULKAN_CAMERA_H
#define VULKAN_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// model matrix - from objects local - world coords
// view matrix - from world space - camera space
// projection - from camera to screen

class Camera {
public:
    constexpr static glm::vec3 worldUp = glm::vec3(0.0, 0.0, 1.0);

    Camera(float FOV, float aspectRatio, float nearPlane, float farPlane);

    void setPosition(const glm::vec3& position) { mPosition = position; }
    void setPitch(double pitch) { mPitch = pitch; }
    void setYaw(double yaw) { mYaw = yaw; }

    glm::vec3 position() const { return mPosition; }
    glm::vec3 forward() const;
    glm::vec3 right() const { return glm::normalize(glm::cross(forward(), worldUp)); }
    glm::vec3 up() const { return glm::normalize(glm::cross(right(), forward())); }

    double pitch() { return mPitch; }
    double yaw() { return mYaw; }

    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix() const { return mProjectionMatrix; }

private:
    void calculateProjection(float FOV, float aspectRatio, float nearPlane, float farPlane);

    glm::vec3 mPosition;
    double mPitch;
    double mYaw;
    glm::mat4 mProjectionMatrix;
};


#endif //VULKAN_CAMERA_H
