//
// Created by auser on 5/5/25.
//

#include "AbsLoader.h"

#include <unordered_map>

static std::unordered_map<AbsLoader::Type, std::string> typeStringMap = {
        {AbsLoader::UNKNOWN, "Unknown" },
        {AbsLoader::ASSIMP , "Assimp"  },
        {AbsLoader::GLTF   , "glTF"    },
        {AbsLoader::OBJ    , "obj"     },
        {AbsLoader::FBX    , "fbx"     }
};

AbsLoader::AbsLoader(std::string modelPath): mType(UNKNOWN), mModelPath(std::move(modelPath)),
                                             mVertices(), mIndices() {}

AbsLoader::AbsLoader(Type type, std::string modelPath): mType(type), mModelPath(std::move(modelPath)),
                                                        mVertices(), mIndices() {}


std::string AbsLoader::getTypeString() {
    return typeStringMap[mType];
}

static glm::vec3 min(const glm::vec3& v1, const glm::vec3& v2) {
    return {
      std::min(v1.x, v2.x),
      std::min(v1.y, v2.y),
      std::min(v1.z, v2.z)
    };
}

static glm::vec3 max(const glm::vec3& v1, const glm::vec3& v2) {
    return {
            std::max(v1.x, v2.x),
            std::max(v1.y, v2.y),
            std::max(v1.z, v2.z)
    };
}

void AbsLoader::computeBBox() {
    for (const auto& vert: vertices()) {
        mBBox.min = min(mBBox.min, vert.pos);
        mBBox.max = max(mBBox.max, vert.pos);
    }
}