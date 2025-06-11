//
// Created by auser on 5/31/25.
//

#include "DirectionalLightBuffer.h"

#include <cstring>
DirectionalLightBuffer::DirectionalLightBuffer(Context* context, Camera* camera):
        UniformBuffer(context, camera){
    createUniformBuffers();
}
auto lightPos = glm::vec3(0.0f, 100.0f, 1.0f);

void DirectionalLightBuffer::updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) {
    DirectionalLight lightData = {};
    lightData.color = glm::vec4(1.0f);
    //lightPos += glm::vec3(0,1,0);
    auto lightTarget = glm::vec3(0);
    auto up = glm::vec3(0.0f, 1.0f, 0.0f);
    lightData.direction = glm::vec4(glm::normalize(lightTarget - lightPos), 1.0f);
    lightData.view = glm::lookAt(lightPos, lightPos + glm::vec3(lightData.direction), up);

    float nearPlane = 0.1f;
    float farPlane = 200.0f;
    float xOrthoHalfSize = 320.0f;
    float yOrthoHalfSize = 320.0f;

    lightData.proj = glm::ortho(
            -xOrthoHalfSize, xOrthoHalfSize,
            -yOrthoHalfSize, yOrthoHalfSize,
            nearPlane, farPlane
    );

    lightData.proj[1][1] *= -1;
    // light.shadowBias = 0.005f;

    std::memcpy(mUniformBuffersMapped[currentImage], &lightData, getSize());
}
