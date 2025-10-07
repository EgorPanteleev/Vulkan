//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_VULKANMODELLOADER_H
#define VULKAN_VULKANMODELLOADER_H

#include "Loader.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"
#include "CubeMapImage.hpp"

struct VulkanTextures {
    VulkanTextures(): mTextures() {}

    Texture*& operator[](int ind) { return mTextures[ind]; }
    std::array<Texture*, cm::Texture::UNKNOWN> mTextures;
};

class VulkanModelLoader: public cm::Loader {
public:
    VulkanModelLoader(Context* context, std::string modelPath, const std::vector<std::string>& skyBoxPaths);
    VulkanModelLoader(Context* context, std::string modelPath);
    ~VulkanModelLoader();
    const std::vector<Vertex>& vulkanVertices() { return mVulkanVertices; }
    const std::vector<VulkanTextures>& vulkanTextures() { return mVulkanTextures; }
    CubeMapImage* getSkyBox() { return mSkyBox.get(); }
    bool load() override;

    bool loadGeometry();
    bool loadMaterials();
private:

    Context* mContext;
    std::vector<Vertex> mVulkanVertices;
    std::vector<VulkanTextures> mVulkanTextures;
    std::unique_ptr<CubeMapImage> mSkyBox;
};


#endif //VULKAN_VULKANMODELLOADER_H
