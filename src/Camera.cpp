//
// Created by auser on 5/4/25.
//

#include "Camera.h"

Camera::Camera(): mPosition(2.0f, 2.0f, 2.0f), mTarget(0.0f), mUp(0.0f, 0.0f, 1.0f),
                  mYaw(30.0f), mPitch(30.0f), mZoom(45.0f) {
    updatePosition();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(mPosition, mTarget, mUp);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    glm::mat4 proj = glm::perspective(glm::radians(mZoom), aspectRatio, 0.1f, 100.0f);
    proj[1][1] *= -1;
    return proj;
}

void Camera::rotate(float dx, float dy) {
    mYaw   += dx;
    mPitch += dy;
    mPitch = glm::clamp(mPitch, -89.0f, 89.0f);

    updatePosition();
}


void Camera::zoomBy(float delta) {
    mZoom -= delta;
    mZoom = glm::clamp(mZoom, 1.0f, 90.0f);
}

void Camera::updatePosition() {
    float radius = glm::length(mPosition - mTarget);
    float radYaw = glm::radians(mYaw);
    float radPitch = glm::radians(mPitch);

    mPosition.x = mTarget.x + radius * cos(radPitch) * cos(radYaw);
    mPosition.y = mTarget.y + radius * cos(radPitch) * sin(radYaw);
    mPosition.z = mTarget.z + radius * sin(radPitch);
}