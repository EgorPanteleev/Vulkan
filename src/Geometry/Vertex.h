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
    Vertex();
    Vertex( const glm::vec3& pos, const glm::vec3& color, const glm::vec2& texCoord, const glm::vec3& normal );
    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();

    bool operator==(const Vertex& other) const;

    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            size_t h1 = std::hash<glm::vec3>()(vertex.pos);
            size_t h2 = std::hash<glm::vec3>()(vertex.color);
            size_t h3 = std::hash<glm::vec2>()(vertex.texCoord);
            size_t h4 = std::hash<glm::vec3>()(vertex.normal);

            return ((h1 ^
                     (h2 << 1)) >> 1) ^
                   (h3 << 1) ^
                   (h4 << 1);
        }
    };
}


#endif //VULKAN_VERTEX_H
