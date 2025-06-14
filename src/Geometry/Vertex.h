//
// Created by auser on 5/4/25.
//

#ifndef VULKAN_VERTEX_H
#define VULKAN_VERTEX_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>


class Vertex {
public:
    static VkVertexInputBindingDescription getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

    bool operator==(const Vertex& other) const;

    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;
    uint32_t texIndex;
};

#endif //VULKAN_VERTEX_H
