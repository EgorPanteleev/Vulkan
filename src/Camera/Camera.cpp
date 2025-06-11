//
// Created by auser on 5/4/25.
//

#include "Camera.h"

Camera::Camera(): mPosition(0), mOrientation(glm::quat(0, 0, 0, 1)), mUp(0, 0, 1) {
}

Camera::Camera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up,
               float FOV, float aspectRatio, float nearPlane, float farPlane):
               mPosition(pos), mOrientation(glm::lookAt(pos, target, up)), mUp(up) {
    calculateProjection(FOV, aspectRatio, nearPlane, farPlane);
}

void Camera::calculateProjection(float FOV, float aspectRatio, float nearPlane, float farPlane) {
    mProjectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
    mProjectionMatrix[1][1] *= -1;
}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(mPosition, mPosition + forward(), up());
}

void Camera::move(float forward_, float right_, float up_) {
    mPosition += forward_ * forward() + right_ * right() + up_ * up();
}

void Camera::rotate(float pitch, float yaw, float roll) {
    glm::quat pitchQuat = glm::angleAxis(glm::radians(pitch), right());
    glm::quat yawQuat = glm::angleAxis(glm::radians(yaw), mUp);
    glm::quat rollQuat  = glm::angleAxis(glm::radians(roll), forward());
    mOrientation = glm::normalize(rollQuat * pitchQuat * yawQuat * mOrientation);
}