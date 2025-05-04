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
    Vertex( const glm::vec3& pos, const glm::vec3& color, glm::vec2 texCoord );
    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

    bool operator==(const Vertex& other) const;

    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
};

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}


#endif //VULKAN_VERTEX_H
