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
    glm::vec2 texCoord0;
    glm::vec2 texCoord1;
    glm::vec3 normal;
};

inline void hashCombine(std::size_t& seed, std::size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            std::size_t seed = 0;
            hashCombine(seed, std::hash<glm::vec3>{}(vertex.pos));
            hashCombine(seed, std::hash<glm::vec3>{}(vertex.color));
            hashCombine(seed, std::hash<glm::vec2>{}(vertex.texCoord0));
            hashCombine(seed, std::hash<glm::vec2>{}(vertex.texCoord1));
            hashCombine(seed, std::hash<glm::vec3>{}(vertex.normal));
            return seed;
        }
    };
}


#endif //VULKAN_VERTEX_H
