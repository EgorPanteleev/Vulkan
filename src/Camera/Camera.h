//
// Created by auser on 5/4/25.
//

#ifndef VULKAN_CAMERA_H
#define VULKAN_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
    Camera();
    Camera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up,
           float FOV, float aspectRatio, float nearPlane, float farPlane);

    glm::vec3 forward() const { return glm::normalize(mOrientation * glm::vec3(0, 0, -1)); }
    glm::vec3 right() const { return glm::normalize(mOrientation * glm::vec3(1, 0, 0)); }
    glm::vec3 up() const { return glm::normalize(mOrientation * glm::vec3(0, 1, 0)); }
    glm::vec3 position() const { return mPosition; }
    glm::mat4 projectionMatrix() const { return mProjectionMatrix; }
    glm::mat4 viewMatrix() const;

    void move(float forward_, float right_, float up_);
    void rotate(float pitch, float yaw, float roll);

private:
    void calculateProjection(float FOV, float aspectRatio, float nearPlane, float farPlane);
    void initOrientation(const glm::vec3& target);

    glm::vec3 mPosition;
    glm::quat mOrientation;
    glm::vec3 mUp;

    glm::mat4 mProjectionMatrix;
};


#endif //VULKAN_CAMERA_H
