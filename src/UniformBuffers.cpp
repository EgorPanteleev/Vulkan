//
// Created by auser on 4/5/25.
//

#include "UniformBuffers.h"
#include "MessageLogger.h"
#include "Utils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <cstring>

UniformBuffers::UniformBuffers(Context* context): mContext(context) {
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
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float) extent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    std::memcpy(mUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}