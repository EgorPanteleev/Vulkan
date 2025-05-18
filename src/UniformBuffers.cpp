//
// Created by auser on 4/5/25.
//

#include "UniformBuffers.h"
#include "MessageLogger.h"
#include "Utils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <cstring>

UniformBuffers::UniformBuffers(Context* context, Camera* camera): mContext(context), mCamera(camera) {
    createUniformBuffers();
}

UniformBuffers::~UniformBuffers() {
    for (size_t i = 0; i < mContext->maxFramesInFlight(); ++i) {
        vmaUnmapMemory(mContext->allocator(), mBuffersAllocation[i]);
        vmaDestroyBuffer(mContext->allocator(), mUniformBuffers[i], mBuffersAllocation[i]);
    }
}


void UniformBuffers::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    mUniformBuffers.resize(mContext->maxFramesInFlight());
    mBuffersAllocation.resize(mContext->maxFramesInFlight());
    mUniformBuffersMapped.resize(mContext->maxFramesInFlight());

    for (size_t i = 0; i < mContext->maxFramesInFlight(); ++i) {
        Utils::createBuffer(mContext->allocator(), mBuffersAllocation[i], VMA_MEMORY_USAGE_CPU_ONLY,
                            bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, mUniformBuffers[i] );

        vmaMapMemory(mContext->allocator(), mBuffersAllocation[i], &mUniformBuffersMapped[i]);
    }
}

void UniformBuffers::updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) {
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ubo.view = mCamera->viewMatrix();
    ubo.proj = mCamera->projectionMatrix();
    std::memcpy(mUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}