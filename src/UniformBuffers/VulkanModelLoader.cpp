//
// Created by auser on 6/12/25.
//

#include "VulkanModelLoader.h"

#include <utility>

VulkanModelLoader::VulkanModelLoader(std::string modelPath): AssimpLoader(std::move(modelPath)) {

}

bool VulkanModelLoader::load() {
    if (!AssimpLoader::load()) return false;
    mVulkanVertices.reserve(mVertices.size());
    for ( size_t i = 0; i < mVertices.size(); ++i) {
        Vertex vert{
          .pos = mVertices[i].pos,
          .color = mVertices[i].color,
          .texCoord = mVertices[i].texCoord0,
          .normal = mVertices[i].normal,
        };
        mVulkanVertices.push_back(vert);
    }
    return true;
}