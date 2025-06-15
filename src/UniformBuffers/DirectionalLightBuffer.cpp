//
// Created by auser on 5/31/25.
//

#include "DirectionalLightBuffer.h"

#include <cstring>
DirectionalLightBuffer::DirectionalLightBuffer(Context* context, Camera* camera):
        UniformBuffer(context, camera){
    createUniformBuffers();
}

void DirectionalLightBuffer::updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) {
    DirectionalLight lightData = {};
    lightData.color = glm::vec4(1.0f);
    auto lightPos = glm::vec3(0.0f, 1500.0f, 0.0f);
    auto direction =  glm::vec3(0.0f, -1.0f, 0.0f);
    lightData.direction = glm::vec4(direction, 1.0f);
    auto up = glm::vec3(-1.0f, 0.0f, 0.0f);

    glm::mat4 view = glm::lookAt(
            lightPos,
            lightPos + direction,
            up
    );

    float nearPlane = 10.0f;
    float farPlane = 2000.0f;
    float xOrthoHalfSize = 2000.0f;
    float yOrthoHalfSize = 3000.0f;

    glm::mat4 proj = glm::ortho(
            -xOrthoHalfSize, xOrthoHalfSize,
            -yOrthoHalfSize, yOrthoHalfSize,
            nearPlane, farPlane
    );

    proj[1][1] *= -1.0f;

    lightData.VPMatrix = proj * view;
    // light.shadowBias = 0.005f;

    std::memcpy(mUniformBuffersMapped[currentImage], &lightData, getSize());
}
