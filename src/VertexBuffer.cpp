//
// Created by auser on 4/5/25.
//

#include "VertexBuffer.h"
#include "MessageLogger.h"
#include "Utils.h"

#include <array>
#include <cstring>

Vertex::Vertex( const glm::vec2& pos, const glm::vec3& color ): pos(pos), color(color) {}

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);
    return attributeDescriptions;
}

VertexBuffer::VertexBuffer(Context* context): mContext(context) {
//    mVertices = { { {0.0f, -0.5f}, {1.0f, 0.0f, 0.0f} },
//                  { {0.5f, 0.5f }, {0.0f, 1.0f, 0.0f} },
//                  { {-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f} } };

    mVertices = { { {-0.5f , -0.5f}, {1.0f, 0.0f, 0.0f} },
                  { { 0.5f , -0.5f}, {0.0f, 1.0f, 0.0f} },
                  { { 0.5f , 0.5f }, {0.0f, 0.0f, 1.0f} },
                  { { -0.5f, 0.5f }, {1.0f, 1.0f, 1.0f} } };
    mIndices = { 0, 1, 2, 2, 3, 0 };
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

    Utils::copyDataToBuffer(mContext->allocator(), allocation, bufferSize, mVertices.data() );

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
    Utils::copyDataToBuffer(mContext->allocator(), allocation, bufferSize, mIndices.data());

    Utils::createBuffer(mContext->allocator(), mIndexBufferAllocation, VMA_MEMORY_USAGE_AUTO,
                        bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, mIndexBuffer);
    Utils::copyBuffer(mContext, stagingBuffer, mIndexBuffer, bufferSize);

    vmaDestroyBuffer(mContext->allocator(), stagingBuffer, allocation);
}
