//
// Created by auser on 5/4/25.
//

#include "Camera.h"

Camera::Camera(float FOV, float aspectRatio, float nearPlane, float farPlane):
               mPosition(0.0, 0.0, 3.0), mPitch(0.0), mYaw(0.0){;
    calculateProjection(FOV, aspectRatio, nearPlane, farPlane);
}

glm::vec3 Camera::forward() const {
    glm::vec3 forward;
    forward.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    forward.y = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    forward.z = sin(glm::radians(mPitch));
    return glm::normalize(forward);
}

glm::mat4 Camera::viewMatrix() const {
//    glm::vec3 direction;
//    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//    direction.y = sin(glm::radians(pitch));
//    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//    glm::vec3 front = glm::normalize(direction);

    return glm::lookAt(mPosition, mPosition + forward(), worldUp);
}

void Camera::calculateProjection(float FOV, float aspectRatio, float nearPlane, float farPlane) {
    mProjectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
    mProjectionMatrix[1][1] *= -1;
}