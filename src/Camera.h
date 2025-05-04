//
// Created by auser on 5/4/25.
//

#ifndef VULKAN_CAMERA_H
#define VULKAN_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();

    float zoom() { return mZoom; }

    glm::mat4 getViewMatrix() const;

    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void rotate(float dx, float dy);

    void zoomBy(float delta);
private:

    void updatePosition();

    glm::vec3 mPosition;
    glm::vec3 mTarget;
    glm::vec3 mUp;

    float mYaw;
    float mPitch;
    float mZoom;
};


#endif //VULKAN_CAMERA_H
