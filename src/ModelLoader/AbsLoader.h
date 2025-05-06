//
// Created by auser on 5/5/25.
//

#ifndef VULKAN_ABSLOADER_H
#define VULKAN_ABSLOADER_H

#include "Vertex.h"

#include <vector>
#include <string>

class AbsLoader {
public:
    enum Type {
        UNKNOWN,
        ASSIMP,
        OBJ,
        GLTF
    };

    AbsLoader(std::string modelPath);
    AbsLoader(Type type, std::string modelPath);
    virtual ~AbsLoader() = default;

    Type getType() { return mType; }
    std::string getTypeString();

    [[nodiscard]] const std::vector<Vertex>& vertices() const { return mVertices; }
    [[nodiscard]] const std::vector<uint32_t>& indices() const { return mIndices; }

    virtual bool load() = 0;

protected:
    Type mType;
    std::string mModelPath;
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;
};


#endif //VULKAN_ABSLOADER_H
