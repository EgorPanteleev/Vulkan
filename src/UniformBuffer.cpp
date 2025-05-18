//
// Created by auser on 4/5/25.
//

#include "UniformBuffer.h"
#include "MessageLogger.h"
#include "Utils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <cstring>

UniformBuffer::UniformBuffer(Context* context, Camera* camera): mContext(context), mCamera(camera) {
}

UniformBuffer::~UniformBuffer() {
    for (size_t i = 0; i < mContext->maxFramesInFlight(); ++i) {
        vmaUnmapMemory(mContext->allocator(), mBuffersAllocation[i]);
        vmaDestroyBuffer(mContext->allocator(), mUniformBuffers[i], mBuffersAllocation[i]);
    }
}


void UniformBuffer::createUniformBuffers() {
    VkDeviceSize bufferSize = getSize();

    mUniformBuffers.resize(mContext->maxFramesInFlight());
    mBuffersAllocation.resize(mContext->maxFramesInFlight());
    mUniformBuffersMapped.resize(mContext->maxFramesInFlight());

    for (size_t i = 0; i < mContext->maxFramesInFlight(); ++i) {
        Utils::createBuffer(mContext->allocator(), mBuffersAllocation[i], VMA_MEMORY_USAGE_CPU_ONLY,
                            bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, mUniformBuffers[i] );

        vmaMapMemory(mContext->allocator(), mBuffersAllocation[i], &mUniformBuffersMapped[i]);
    }
}
