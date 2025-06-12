//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_MODELVERTEX_H
#define VULKAN_MODELVERTEX_H

#include <glm/glm.hpp>

struct ModelVertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord0;
    glm::vec2 texCoord1;
    glm::vec3 normal;
};


#endif //VULKAN_MODELVERTEX_H
