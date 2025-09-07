//
// Created by auser on 4/5/25.
//

#ifndef VULKAN_VERTEXBUFFER_H
#define VULKAN_VERTEXBUFFER_H

#include <vector>

#include "Context.hpp"
#include "Vertex.hpp"
#include "VulkanModelLoader.hpp"

template <typename VertexType>
class VertexBuffer {
public:
    VertexBuffer(Context* context, const std::vector<VertexType>& vertices,
                 const std::vector<uint32_t>& indices): mContext(context) {
        mVertices = vertices;
        mIndices = indices;
        createVertexBuffer();
        createIndexBuffer();
    }

    ~VertexBuffer() {
        vmaDestroyBuffer(mContext->allocator(), mIndexBuffer, mIndexBufferAllocation);
        vmaDestroyBuffer(mContext->allocator(), mVertexBuffer, mVertexBufferAllocation);
    }

    std::vector<VertexType>& vertices() { return mVertices; }
    std::vector<uint32_t>& indices() { return mIndices; }
    VkBuffer vertexBuffer() { return mVertexBuffer; }
    VkBuffer indexBuffer() { return mIndexBuffer; }
private:
    void createVertexBuffer() {
        VkDeviceSize bufferSize = sizeof(VertexType) * mVertices.size();

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
    void createIndexBuffer() {
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

    Context* mContext;
    std::vector<VertexType> mVertices;
    VkBuffer mVertexBuffer;
    VmaAllocation mVertexBufferAllocation;
    std::vector<uint32_t> mIndices;
    VkBuffer mIndexBuffer;
    VmaAllocation mIndexBufferAllocation;
};

#endif //VULKAN_VERTEXBUFFER_H
