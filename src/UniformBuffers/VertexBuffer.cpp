//
// Created by auser on 4/5/25.
//

#include "VertexBuffer.h"
#include "MessageLogger.h"
#include "Utils.h"
#include "ModelLoader.h"

VertexBuffer::VertexBuffer(Context* context, const std::string& path): mContext(context) {
//    mVertices = { { {-0.5f , -0.5f, 0.0f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
//                  { { 0.5f , -0.5f, 0.0f }, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
//                  { { 0.5f , 0.5f, 0.0f }, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
//                  { { -0.5f, 0.5f, 0.0f }, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },
//
//                  { {-0.5f , -0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
//                  { { 0.5f , -0.5f, -0.5f }, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
//                  { { 0.5f , 0.5f, -0.5f }, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
//                  { { -0.5f, 0.5f, -0.5f }, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} } };
//
//    for ( auto& vert: mVertices ) {
//        vert.pos += glm::vec3( 100, 100, 100 );
//    }
//
//    mIndices = { 0, 1, 2, 2, 3, 0,
//                 4, 5, 6, 6, 7, 4 };

    auto modelLoader = ModelLoader::createLoader(path);
    modelLoader->load();

    mVertices = modelLoader->vertices();
    mIndices = modelLoader->indices();

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
