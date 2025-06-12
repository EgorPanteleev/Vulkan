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