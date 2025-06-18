//
// Created by auser on 5/4/25.
//

#include "Camera.h"

Camera::Camera(): mPosition(0), mOrientation(glm::quat(0, 0, 0, 1)), mUp(0, 0, 1) {
}

Camera::Camera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up,
               float FOV, float aspectRatio, float nearPlane, float farPlane):
               mPosition(pos), mOrientation(0, 0, 0, 1), mUp(up) {
    calculateProjection(FOV, aspectRatio, nearPlane, farPlane);
    initOrientation(target);
}

void Camera::calculateProjection(float FOV, float aspectRatio, float nearPlane, float farPlane) {
    mProjectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
    //mProjectionMatrix = glm::ortho(-2000.f, 2000.f, -2000.f, 2000.f, -2000.f, 2000.f);
    mProjectionMatrix[1][1] *= -1;
}

void Camera::initOrientation(const glm::vec3& target) {
    glm::vec3 forward_ = glm::normalize(target - mPosition);
    glm::vec3 right_   = glm::normalize(glm::cross(forward_, mUp));
    glm::vec3 camUp_   = glm::cross(right_, forward_);
    glm::mat3 rotationMatrix(right_, camUp_, -forward_);
    mOrientation = glm::quat_cast(rotationMatrix);
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