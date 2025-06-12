//
// Created by auser on 5/4/25.
//

#ifndef VULKAN_VERTEX_H
#define VULKAN_VERTEX_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <array>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

class Vertex {
public:
    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();

    bool operator==(const Vertex& other) const;

    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

#endif //VULKAN_VERTEX_H
