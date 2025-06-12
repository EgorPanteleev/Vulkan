//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_VULKANMODELLOADER_H
#define VULKAN_VULKANMODELLOADER_H

#include "AssimpLoader.h"
#include "Vertex.h"
#include "Texture.h"

class VulkanModelLoader: public AssimpLoader {
public:
    VulkanModelLoader(std::string modelPath);
    const std::vector<Vertex>& vulkanVertices() { return mVulkanVertices; }
    bool load() override;
private:
    std::vector<Vertex> mVulkanVertices;
};


#endif //VULKAN_VULKANMODELLOADER_H
