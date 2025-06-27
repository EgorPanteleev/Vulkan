//
// Created by auser on 5/5/25.
//

#ifndef VULKAN_ABSLOADER_H
#define VULKAN_ABSLOADER_H

#include "ModelVertex.h"
#include "ModelMesh.h"
#include "ModelMaterial.h"
#include "BBox.h"

#include <vector>
#include <string>

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

    Type type() { return mType; }
    std::string getTypeString();
    BBox bbox() { return mBBox; }

    [[nodiscard]] const std::vector<ModelMesh>& meshes() const { return mMeshes; }
    [[nodiscard]] const std::vector<ModelMaterial>& materials() const { return mMaterials; }
    [[nodiscard]] const std::vector<ModelVertex>& vertices() const { return mVertices; }
    [[nodiscard]] const std::vector<uint32_t>& indices() const { return mIndices; }

    virtual bool load() = 0;

protected:
    void computeBBox();

    Type mType;
    std::string mModelPath;
    std::vector<ModelMesh> mMeshes;
    std::vector<ModelMaterial> mMaterials;
    std::vector<ModelVertex> mVertices;
    std::vector<uint32_t> mIndices;
    BBox mBBox;
};


#endif //VULKAN_ABSLOADER_H
