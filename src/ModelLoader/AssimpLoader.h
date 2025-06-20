//
// Created by auser on 5/6/25.
//

#ifndef VULKAN_ASSIMPLOADER_H
#define VULKAN_ASSIMPLOADER_H

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "AbsLoader.h"

class AssimpLoader: public AbsLoader {
public:
    AssimpLoader(std::string modelPath);

    bool load() override;
private:
    bool loadScene();
    bool loadGeometry();
    bool loadMaterials();

    template<typename VertexType>
    void loadMesh(std::vector<VertexType>& vertices, std::vector<uint32_t>& indices, uint meshIndex);

    template<typename VertexType>
    void optimizeMesh(std::vector<VertexType>& vertices, std::vector<uint32_t>& indices, uint meshIndex);

    void loadTextures(uint materialIndex);

    void loadTexture(ModelTexture::Type textureType, uint materialIndex);

    void loadColors(uint materialIndex);

    const aiScene* mScene;
};


#endif //VULKAN_ASSIMPLOADER_H
