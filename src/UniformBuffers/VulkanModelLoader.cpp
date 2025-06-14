//
// Created by auser on 6/12/25.
//

#include "VulkanModelLoader.h"

#include <utility>

VulkanModelLoader::VulkanModelLoader(Context* context, std::string modelPath): AssimpLoader(std::move(modelPath)),
                                                                               mContext(context) {
}

VulkanModelLoader::~VulkanModelLoader() {
    for (auto& vulkanTextureVec: mVulkanTextures) {
        for (auto& mTexture : vulkanTextureVec.mTextures){
            delete mTexture;
            mTexture = nullptr;
        }
    }
}

bool VulkanModelLoader::load() {
    return AssimpLoader::load() &&
           loadGeometry()       &&
           loadMaterials();
}

bool VulkanModelLoader::loadGeometry() {
    mVulkanVertices.reserve(mVertices.size());
    for (size_t i = 0; i < mMeshes.size(); ++i) {
        ModelMesh mesh = mMeshes[i];
        for (size_t j = 0; j < mesh.numVertices; ++j) {
            ModelVertex modelVertex = mVertices[mesh.baseVertex + j];
            Vertex vert{
                    .pos = modelVertex.pos,
                    .color = modelVertex.color,
                    .texCoord = modelVertex.texCoord0,
                    .normal = modelVertex.normal,
                    .texIndex = (uint32_t) mesh.materialIndex
            };
            mVulkanVertices.push_back(vert);
        }
    }
    return true;
}

bool VulkanModelLoader::loadMaterials() {
    mVulkanTextures.resize(mMaterials.size());
    for (size_t i = 0; i < mMaterials.size(); ++i) {
        ModelMaterial material = mMaterials[i];
        VulkanTextures& vulkanTextures = mVulkanTextures[i];
        for (int tex = 0; tex < (int) ModelTexture::UNKNOWN; ++tex) {
            ModelTexture texture = material.mTextures[tex];
            Texture*& vulkanTexture = vulkanTextures[tex];
            vulkanTexture = new Texture(mContext, true);
            if (texture.embedded) {
                vulkanTexture->load(texture.data, texture.bufferSize);
            } else {
                vulkanTexture->load(texture.path);
            }
        }
    }
    return true;
}