//
// Created by auser on 5/6/25.
//

#include "AssimpLoader.h"
#include "MessageLogger.h"

#include <stdexcept>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <assimp/postprocess.h>

AssimpLoader::AssimpLoader(std::string modelPath): AbsLoader(ASSIMP, std::move(modelPath)) {}

bool AssimpLoader::load() {
    Assimp::Importer importer;
    unsigned int flags = aiProcess_Triangulate |
                         aiProcess_FlipUVs |
                         aiProcess_GenNormals;
    const aiScene* scene = importer.ReadFile(mModelPath, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        const aiMesh* mesh = scene->mMeshes[m];

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                // Extract vertex data from the mesh
                unsigned int assimpIndex = face.mIndices[j];

                // Position
                glm::vec3 pos;
                pos.x = mesh->mVertices[assimpIndex].x;
                pos.y = mesh->mVertices[assimpIndex].y;
                pos.z = mesh->mVertices[assimpIndex].z;

                glm::vec3 normal;
                if (mesh->HasNormals()) {
                    aiVector3D aiNormal = mesh->mNormals[assimpIndex];
                    normal = {aiNormal.x, aiNormal.y, aiNormal.z};
                } else {
                    normal = {0, 0, 0};
                }

                // Color (default to white if missing)
                glm::vec3 color(1.0f);
                if (mesh->HasVertexColors(0)) {
                    color.r = mesh->mColors[0][assimpIndex].r;
                    color.g = mesh->mColors[0][assimpIndex].g;
                    color.b = mesh->mColors[0][assimpIndex].b;
                }

                // Texture coordinates (default to (0,0) if missing)
                glm::vec2 texCoord(0.0f);
                if (mesh->HasTextureCoords(0)) {
                    texCoord.x = mesh->mTextureCoords[0][assimpIndex].x;
                    texCoord.y = mesh->mTextureCoords[0][assimpIndex].y;
                }

                // Create a Vertex instance
                Vertex vertex(pos, color, texCoord, normal);

                // Check if this vertex is already in the map
                if (uniqueVertices.find(vertex) != uniqueVertices.end()) {
                    // Reuse existing index
                    mIndices.push_back(uniqueVertices[vertex]);
                } else {
                    // Add new vertex and update the map
                    uint32_t newIndex = static_cast<uint32_t>(mVertices.size());
                    mVertices.push_back(vertex);
                    uniqueVertices[vertex] = newIndex;
                    mIndices.push_back(newIndex);
                }
            }
        }
    }
    INFO << "Vertices size - " << mVertices.size();
    INFO << "Indices size - " << mIndices.size();

    return true;
}
