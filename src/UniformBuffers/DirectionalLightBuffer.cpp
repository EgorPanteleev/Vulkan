//
// Created by auser on 5/31/25.
//

#include "DirectionalLightBuffer.h"

#include <cstring>

#include "MessageLogger.h"

DirectionalLightBuffer::DirectionalLightBuffer(Context* context, Camera* camera, const BBox& sceneBBox, const glm::vec3& dir):
        UniformBuffer(context, camera), mSceneBBox(sceneBBox), mDirection(dir){
    createUniformBuffers();
}

void DirectionalLightBuffer::updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) {
    glm::vec3 min = mSceneBBox.min;
    glm::vec3 max = mSceneBBox.max;

    glm::vec3 center = (min + max) * 0.5f;
    float radius = glm::length(max - center);
    glm::vec eye = center - mDirection * radius;

    glm::vec3 up = {0.0f, 1.0f, 1.0f};
    glm::mat4 view = glm::lookAt(
            eye,
            center,
            up
    );

    std::vector<glm::vec3> corners = {
            {min.x, min.y, min.z},
            {min.x, min.y, max.z},
            {min.x, max.y, min.z},
            {min.x, max.y, max.z},
            {max.x, min.y, min.z},
            {max.x, min.y, max.z},
            {max.x, max.y, min.z},
            {max.x, max.y, max.z},
    };

    glm::vec3 lightMin(FLT_MAX);
    glm::vec3 lightMax(-FLT_MAX);

    for (const auto& corner: corners) {
        glm::vec4 v = view * glm::vec4(corner, 1.0f);
        lightMin.x = std::min(lightMin.x, v.x);
        lightMin.y = std::min(lightMin.y, v.y);
        lightMin.z = std::min(lightMin.z, v.z);
        lightMax.x = std::max(lightMax.x, v.x);
        lightMax.y = std::max(lightMax.y, v.y);
        lightMax.z = std::max(lightMax.z, v.z);
    }

    float padding = 0.0f;
    float left = lightMin.x - padding;
    float right = lightMax.x + padding;
    float bottom = lightMin.y - padding;
    float top = lightMax.y + padding;
    float near = -lightMax.z;
    float far = -lightMin.z;

    glm::mat4 proj = glm::ortho(left, right, bottom, top, near, far);
//    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.6f, near, far);
    //proj[1][1] *= -1.0f;

    DirectionalLight lightData = {
            .VPMatrix = proj * view,
            .color = glm::vec4(1.0f),
            .direction = glm::vec4(mDirection, 1.0f)
    };
    // light.shadowBias = 0.005f;

    std::memcpy(mUniformBuffersMapped[currentImage], &lightData, getSize());
}
