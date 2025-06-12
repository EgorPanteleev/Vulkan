//
// Created by auser on 5/5/25.
//

#ifndef VULKAN_ABSLOADER_H
#define VULKAN_ABSLOADER_H

#include "Vertex.h"
#include "Material.h"

#include <vector>
#include <string>



struct BasicMesh {
    uint32_t numIndices = 0;
    uint32_t numVertices = 0;
    uint32_t baseVertex = 0;
    uint32_t baseIndex = 0;
    uint32_t validFaces = 0;
    int materialIndex = -1;
    glm::mat4 transformation;
};

class AbsLoader {
public:
    enum Type {
        UNKNOWN,
        ASSIMP,
        OBJ,
        GLTF,
        FBX
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
    std::vector<BasicMesh> mMeshes;
    std::vector<Material> mMaterials;
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;
};


#endif //VULKAN_ABSLOADER_H
