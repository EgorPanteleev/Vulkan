//
// Created by auser on 4/5/25.
//

#ifndef VULKAN_VERTEXBUFFER_H
#define VULKAN_VERTEXBUFFER_H

#include <glm/glm.hpp>
#include <vector>

#include "Context.h"

struct Vertex {
    Vertex( const glm::vec2& pos, const glm::vec3& color );
    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

    glm::vec2 pos;
    glm::vec3 color;
};

class VertexBuffer {
public:
    VertexBuffer(Context* context);
    ~VertexBuffer();

    std::vector<Vertex>& vertices() { return mVertices; }
    VkBuffer vertexBuffer() { return mVertexBuffer; }
    VkDeviceMemory vertexBufferMemory() { return mVertexBufferMemory; }
private:
    void createVertexBuffer();

    Context* mContext;
    std::vector<Vertex> mVertices;
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
};


#endif //VULKAN_VERTEXBUFFER_H
