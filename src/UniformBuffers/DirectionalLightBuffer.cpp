//
// Created by auser on 5/31/25.
//

#include "DirectionalLightBuffer.h"

#include <cstring>
DirectionalLightBuffer::DirectionalLightBuffer(Context* context, Camera* camera):
        UniformBuffer(context, camera){
    createUniformBuffers();
}
auto lightPos = glm::vec3(0.0f, 1500.0f, 1.0f);

void DirectionalLightBuffer::updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) {
    DirectionalLight lightData = {};
    lightData.color = glm::vec4(1.0f);
    //lightPos += glm::vec3(0,0.01,0);
    auto lightTarget = glm::vec3(0);
    auto up = glm::vec3(0.0f, 1.0f, 0.0f);
    lightData.direction = glm::vec4(glm::normalize(lightTarget - lightPos), 1.0f);
    glm::mat4 view = glm::lookAt(lightPos, lightPos + glm::vec3(lightData.direction), up);

    float nearPlane = 0.1f;
    float farPlane = 2000.0f;
    float xOrthoHalfSize = 1000.0f;
    float yOrthoHalfSize = 1000.0f;

    glm::mat4 proj = glm::ortho(
            -xOrthoHalfSize, xOrthoHalfSize,
            -yOrthoHalfSize, yOrthoHalfSize,
            nearPlane, farPlane
    );

    proj[1][1] *= -1;

    lightData.VPMatrix = proj * view;
    // light.shadowBias = 0.005f;

    std::memcpy(mUniformBuffersMapped[currentImage], &lightData, getSize());
}
