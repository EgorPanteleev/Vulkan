//
// Created by auser on 4/5/25.
//

#include "VertexBuffer.h"
#include "MessageLogger.h"
#include "Utils.h"
#include "VulkanModelLoader.h"

VertexBuffer::VertexBuffer(Context* context, VulkanModelLoader* loader): mContext(context), mLoader(loader) {
    mVertices = loader->vulkanVertices();
    mIndices = loader->indices();

    createVertexBuffer();
    createIndexBuffer();
}

VertexBuffer::~VertexBuffer() {
    vmaDestroyBuffer(mContext->allocator(), mIndexBuffer, mIndexBufferAllocation);
    vmaDestroyBuffer(mContext->allocator(), mVertexBuffer, mVertexBufferAllocation);
}

void VertexBuffer::createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();

    VkBuffer stagingBuffer;
    VmaAllocation allocation;
    Utils::createBuffer(mContext->allocator(), allocation, VMA_MEMORY_USAGE_CPU_ONLY,
                        bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer );

    Utils::copyDataToBuffer(mContext->allocator(), allocation, mVertices.data(), bufferSize );

    Utils::createBuffer(mContext->allocator(), mVertexBufferAllocation, VMA_MEMORY_USAGE_AUTO,
                        bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, mVertexBuffer );
    Utils::copyBuffer(mContext, stagingBuffer, mVertexBuffer, bufferSize);

    vmaDestroyBuffer(mContext->allocator(), stagingBuffer, allocation);
}

void VertexBuffer::createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();

    VkBuffer stagingBuffer;
    VmaAllocation allocation;
    Utils::createBuffer(mContext->allocator(), allocation, VMA_MEMORY_USAGE_CPU_ONLY,
                        bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);
    Utils::copyDataToBuffer(mContext->allocator(), allocation, mIndices.data(), bufferSize);

    Utils::createBuffer(mContext->allocator(), mIndexBufferAllocation, VMA_MEMORY_USAGE_AUTO,
                        bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, mIndexBuffer);
    Utils::copyBuffer(mContext, stagingBuffer, mIndexBuffer, bufferSize);

    vmaDestroyBuffer(mContext->allocator(), stagingBuffer, allocation);
}
