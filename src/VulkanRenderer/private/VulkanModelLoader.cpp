//
// Created by auser on 6/12/25.
//

#include "VulkanModelLoader.hpp"
#include "Message.hpp"
#include "AbsLoader.hpp"

#include <utility>

VulkanModelLoader::VulkanModelLoader(Context* context, std::string modelPath, const std::vector<std::string>& skyBoxPaths):
                                     Loader(std::move(modelPath)), mContext(context) {
    mSkyBox = std::make_unique<CubeMapImage>(context);
    mSkyBox->load({skyBoxPaths});
    CubeMapTransitInfo transitInfo{
        .src = VK_IMAGE_LAYOUT_UNDEFINED,
        .dst = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .layer = 0,
        .layerCount = 6,
    };
    mSkyBox->transit(transitInfo);
}
VulkanModelLoader::VulkanModelLoader(Context* context, std::string modelPath): Loader(std::move(modelPath)),
                                                                               mContext(context) {
}

VulkanModelLoader::~VulkanModelLoader() {
    for (auto& vulkanTextureVec: mVulkanTextures) {
        for (auto& mTexture : vulkanTextureVec.mTextures){
            mTexture->destroy();
            delete mTexture;
            mTexture = nullptr;
        }
    }
    if (mSkyBox) mSkyBox->destroy();
}

bool VulkanModelLoader::load() {
    return Loader::load() &&
           loadGeometry()       &&
           loadMaterials();
}

bool VulkanModelLoader::loadGeometry() {
    const std::vector<cm::Mesh>& meshes = mLoader->meshes();
    const std::vector<cm::Vertex>& vertices = mLoader->vertices();
    mVulkanVertices.reserve(vertices.size());
    for (size_t i = 0; i < meshes.size(); ++i) {
        const cm::Mesh& mesh = meshes[i];
        for (size_t j = 0; j < mesh.numVertices; ++j) {
            const cm::Vertex& modelVertex = vertices[mesh.baseVertex + j];
            Vertex vert{
                    .pos = modelVertex.pos,
                    .color = modelVertex.color,
                    .texCoord = modelVertex.texCoord0,
                    .normal = modelVertex.normal,
                    .tangent = modelVertex.tangent,
                    .texIndex = (uint32_t) mesh.materialIndex * cm::Texture::UNKNOWN
            };
            mVulkanVertices.push_back(vert);
        }
    }
    return true;
}

bool VulkanModelLoader::loadMaterials() {
    const std::vector<cm::Material>& materials = mLoader->materials();
    mVulkanTextures.resize(materials.size());
    for (size_t i = 0; i < materials.size(); ++i) {
        const cm::Material& material = materials[i];
        VulkanTextures& vulkanTextures = mVulkanTextures[i];
        for (int tex = 0; tex < (int) cm::Texture::UNKNOWN; ++tex) {
            const cm::Texture& texture = material.mTextures[tex];
            Texture*& vulkanTexture = vulkanTextures[tex];
            vulkanTexture = new Texture(mContext);
            TextureLoadInfo loadInfo{
                .dataByLevel = texture.mDataByLevel,
                .texFormat = texture.mFormat,
                .generateMipMap = true
            };
            vulkanTexture->load(loadInfo);
        }
    }
    return true;
}