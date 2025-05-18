//
// Created by auser on 5/18/25.
//

#include "ModelUniformBuffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>

ModelUniformBuffer::ModelUniformBuffer(Context* context, Camera* camera): UniformBuffer(context, camera) {
    createUniformBuffers();
}

void ModelUniformBuffer::updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) {
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ubo.view = mCamera->viewMatrix();
    ubo.proj = mCamera->projectionMatrix();
    std::memcpy(mUniformBuffersMapped[currentImage], &ubo, getSize());
}
