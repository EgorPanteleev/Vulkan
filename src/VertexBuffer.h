//
// Created by auser on 4/5/25.
//

#ifndef VULKAN_VERTEXBUFFER_H
#define VULKAN_VERTEXBUFFER_H

#include <glm/glm.hpp>
#include <vector>

#include "Context.h"

struct Vertex {
    Vertex( const glm::vec2& pos, const glm::vec3& color, glm::vec2 texCoord );
    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
};

class VertexBuffer {
public:
    VertexBuffer(Context* context);
    ~VertexBuffer();

    std::vector<Vertex>& vertices() { return mVertices; }
    std::vector<uint32_t>& indices() { return mIndices; }
    VkBuffer vertexBuffer() { return mVertexBuffer; }
    VkBuffer indexBuffer() { return mIndexBuffer; }
private:
    void createVertexBuffer();
    void createIndexBuffer();

    Context* mContext;
    std::vector<Vertex> mVertices;
    VkBuffer mVertexBuffer;
    VmaAllocation mVertexBufferAllocation;
    std::vector<uint32_t> mIndices;
    VkBuffer mIndexBuffer;
    VmaAllocation mIndexBufferAllocation;
};


#endif //VULKAN_VERTEXBUFFER_H
