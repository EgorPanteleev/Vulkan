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
    vkDestroyBuffer(mContext->device(), mIndexBuffer, nullptr);
    vkFreeMemory(mContext->device(), mIndexBufferMemory, nullptr);

    vkDestroyBuffer(mContext->device(), mVertexBuffer, nullptr);
    vkFreeMemory(mContext->device(), mVertexBufferMemory, nullptr);
}

void VertexBuffer::createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Utils::createBuffer(mContext->device(), mContext->physicalDevice(), bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(mContext->device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, mVertices.data(), (size_t) bufferSize);
    vkUnmapMemory(mContext->device(), stagingBufferMemory);

    Utils::createBuffer(mContext->device(), mContext->physicalDevice(), bufferSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        mVertexBuffer, mVertexBufferMemory);
    Utils::copyBuffer(mContext->device(), mContext->physicalDevice(), mContext->surface(),
                      mContext->graphicsQueue(), stagingBuffer, mVertexBuffer, bufferSize);

    vkDestroyBuffer(mContext->device(), stagingBuffer, nullptr);
    vkFreeMemory(mContext->device(), stagingBufferMemory, nullptr);
}

void VertexBuffer::createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Utils::createBuffer(mContext->device(), mContext->physicalDevice(), bufferSize,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(mContext->device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, mIndices.data(), (size_t) bufferSize);
    vkUnmapMemory(mContext->device(), stagingBufferMemory);

    Utils::createBuffer(mContext->device(), mContext->physicalDevice(), bufferSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        mIndexBuffer, mIndexBufferMemory);
    Utils::copyBuffer(mContext->device(), mContext->physicalDevice(), mContext->surface(),
                      mContext->graphicsQueue(), stagingBuffer, mIndexBuffer, bufferSize);

    vkDestroyBuffer(mContext->device(), stagingBuffer, nullptr);
    vkFreeMemory(mContext->device(), stagingBufferMemory, nullptr);
}
