//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_VULKANMODELLOADER_H
#define VULKAN_VULKANMODELLOADER_H

#include "AssimpLoader.h"
#include "Vertex.h"
#include "Texture.h"

struct VulkanTextures {
    VulkanTextures(): mTextures() {}

    Texture*& operator[](int ind) { return mTextures[ind]; }
    std::array<Texture*, ModelTexture::UNKNOWN> mTextures;
};

class VulkanModelLoader: public AssimpLoader {
public:
    VulkanModelLoader(Context* context, std::string modelPath);
    ~VulkanModelLoader();
    const std::vector<Vertex>& vulkanVertices() { return mVulkanVertices; }
    bool load() override;

    bool loadGeometry();
    bool loadMaterials();
private:
    Context* mContext;

    std::vector<Vertex> mVulkanVertices;
    std::vector<VulkanTextures> mVulkanTextures;
};


#endif //VULKAN_VULKANMODELLOADER_H
