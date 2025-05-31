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
    lightData.direction = glm::vec4(glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f)), 0.0f);
    lightData.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.0f, 10.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    lightData.view = glm::lookAt(lightPos, lightPos + glm::vec3(lightData.direction), up);

    lightData.proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);

    // light.shadowBias = 0.005f;

    std::memcpy(mUniformBuffersMapped[currentImage], &lightData, getSize());
}