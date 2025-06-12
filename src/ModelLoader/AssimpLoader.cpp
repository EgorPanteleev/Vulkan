//
// Created by auser on 5/6/25.
//

#include "AssimpLoader.h"
#include "MessageLogger.h"

#include <stdexcept>

#include <assimp/postprocess.h>
#include <meshoptimizer.h>
#include <filesystem>
namespace fs = std::filesystem;

#define ASSIMP_LOAD_FLAGS (aiProcess_JoinIdenticalVertices    | \
                           aiProcess_Triangulate              | \
                           aiProcess_GenSmoothNormals         | \
                           aiProcess_LimitBoneWeights         | \
                           aiProcess_SplitLargeMeshes         | \
                           aiProcess_ImproveCacheLocality     | \
                           aiProcess_RemoveRedundantMaterials | \
                           aiProcess_FindDegenerates          | \
                           aiProcess_FindInvalidData          | \
                           aiProcess_GenUVCoords              | \
                           aiProcess_CalcTangentSpace           )

//#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | \
//                           aiProcess_FlipUVs     | \
//                           aiProcess_GenNormals    )


AssimpLoader::AssimpLoader(std::string modelPath): AbsLoader(ASSIMP, std::move(modelPath)) {}

bool AssimpLoader::load() {
    Assimp::Importer importer;
    mScene = importer.ReadFile(mModelPath, ASSIMP_LOAD_FLAGS);
    if (!mScene) {
        std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
        return false;
    }
//    m_GlobalInverseTransform = mScene->mRootNode->mTransformation;
//    m_GlobalInverseTransform = m_GlobalInverseTransform.Inverse();
    return loadScene();
}

bool AssimpLoader::loadScene() {
    return loadGeometry() &&
           loadMaterials();
}

uint countValidFaces(const aiMesh* Mesh)
{
    uint NumValidFaces = 0;

    for (uint i = 0; i < Mesh->mNumFaces; i++) {
        if (Mesh->mFaces[i].mNumIndices == 3) {
            NumValidFaces++;
        }
    }

    return NumValidFaces;
}

bool AssimpLoader::loadGeometry() {
    mMeshes.resize(mScene->mNumMeshes);
    uint numVertices = 0;
    uint numIndices = 0;
    for (unsigned int i = 0 ; i < mMeshes.size() ; ++i) { //fill meshes data
        mMeshes[i].materialIndex = (int) mScene->mMeshes[i]->mMaterialIndex;
        mMeshes[i].validFaces = countValidFaces(mScene->mMeshes[i]);
        mMeshes[i].numIndices = mMeshes[i].validFaces * 3;
        mMeshes[i].numVertices = mScene->mMeshes[i]->mNumVertices;
        mMeshes[i].baseVertex = numVertices; //TODO for optimized meshes
        mMeshes[i].baseIndex = numIndices;
        numVertices += mScene->mMeshes[i]->mNumVertices;
        numIndices  += mMeshes[i].numIndices;
    }
    mVertices.reserve(numVertices);
    mIndices.reserve(numIndices);

    for (size_t i = 0 ; i < mMeshes.size() ; ++i) {
        std::vector<ModelVertex> vertices;
        std::vector<uint32_t> indices;
        vertices.reserve(numVertices);
        indices.reserve(numIndices);
        loadMesh<ModelVertex>(vertices, indices, i);
        optimizeMesh<ModelVertex>(vertices, indices, i);
        mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
        uint32_t baseVertex = mVertices.size() - vertices.size();
        std::ranges::for_each(indices, [&](uint32_t & ind) { ind += baseVertex; });
        mIndices.insert(mIndices.end(), indices.begin(), indices.end());
    }
    return true;
}

bool AssimpLoader::loadMaterials() {
    mMaterials.resize(mScene->mNumMaterials);
    for (uint i = 0 ; i < mScene->mNumMaterials; ++i) {
        loadTextures(i);
        loadColors(i);
    }
    return true;
}

template<typename VertexType>
void AssimpLoader::loadMesh(std::vector<VertexType>& vertices, std::vector<uint32_t>& indices, uint meshIndex) {
    VertexType vert{};
    const aiMesh* mesh = mScene->mMeshes[meshIndex];
    for (size_t i = 0 ; i < mesh->mNumVertices ; ++i) {
        const aiVector3D& pos = mesh->mVertices[i];
        vert.pos = glm::vec3(pos.x, pos.y, pos.z);

        if (mesh->mNormals) {
            const aiVector3D& normal = mesh->mNormals[i];
            vert.normal = glm::vec3(normal.x, normal.y, normal.z);
        } else {
            vert.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (mesh->HasTextureCoords(0)) {
            const aiVector3D& texCoord = mesh->mTextureCoords[0][i];
            vert.texCoord0 = glm::vec2(texCoord.x, texCoord.y);
        } else {
            vert.texCoord0 = glm::vec2(0.0f);
        }
        if (mesh->HasTextureCoords(1)) {
            const aiVector3D& texCoord = mesh->mTextureCoords[1][i];
            vert.texCoord1 = glm::vec2(texCoord.x, texCoord.y);
        }
        else {
            vert.texCoord1 = glm::vec2(0.0f);
        }

        if (mesh->mColors[0]) {
            const aiColor4D& color = *mesh->mColors[0];
            vert.color = glm::vec3(color.r, color.g, color.b);
        } else {
            vert.color = glm::vec3(1.0f);
        }

        vertices.push_back(vert);
    }

    for (size_t i = 0 ; i < mesh->mNumFaces ; ++i) {
        const aiFace& face = mesh->mFaces[i];
        if (face.mNumIndices != 3) {
            WARNING << "face " << i << " has " << face.mNumIndices << " indices";
            continue;
        }
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
}

template<typename VertexType>
void AssimpLoader::optimizeMesh(std::vector<VertexType>& vertices, std::vector<uint32_t>& indices, uint meshIndex) {
    size_t numIndices = indices.size();
    size_t numVertices = vertices.size();

    std::vector<unsigned int> remap(numIndices);
    size_t optVertexCount = meshopt_generateVertexRemap(remap.data(),    // dst addr
                                                        indices.data(),  // src indices
                                                        numIndices,      // ...and size
                                                        vertices.data(), // src vertices
                                                        numVertices,     // ...and size
                                                        sizeof(VertexType)); // stride
    // Allocate a local index/vertex arrays
    std::vector<uint> optIndices;
    std::vector<VertexType> optVertices;
    optIndices.resize(numIndices);
    optVertices.resize(optVertexCount);
    // Optimization #1: remove duplicate vertices
    meshopt_remapIndexBuffer(optIndices.data(), indices.data(), numIndices, remap.data());
    meshopt_remapVertexBuffer(optVertices.data(), vertices.data(), numVertices, sizeof(VertexType), remap.data());
    // Optimization #2: improve the locality of the vertices
    meshopt_optimizeVertexCache(optIndices.data(), optIndices.data(), numIndices, optVertexCount);
    // Optimization #3: reduce pixel overdraw
    meshopt_optimizeOverdraw(optIndices.data(), optIndices.data(), numIndices, &(optVertices[0].pos.x),
                             optVertexCount, sizeof(VertexType), 1.05f);
    // Optimization #4: optimize access to the vertex buffer
    meshopt_optimizeVertexFetch(optVertices.data(), optIndices.data(), numIndices, optVertices.data(),
                                optVertexCount, sizeof(VertexType));
    // Optimization #5: create a simplified version of the model
    float threshold = 1.0f;
    size_t targetIndexCount = (size_t)(numIndices * threshold);

    float targetError = 0.0f;
    std::vector<unsigned int> SimplifiedIndices(optIndices.size());
    size_t optIndexCount = meshopt_simplify(SimplifiedIndices.data(), optIndices.data(), numIndices,
                                            &optVertices[0].pos.x, optVertexCount, sizeof(VertexType),
                                            targetIndexCount, targetError);

    static int num_indices = 0;
    num_indices += (int) numIndices;
    static int opt_indices = 0;
    opt_indices += (int) optIndexCount;
    SimplifiedIndices.resize(optIndexCount);

    // Concatenate the local arrays into the class attributes arrays
    indices = SimplifiedIndices;
    vertices = optVertices;

    mMeshes[meshIndex].numIndices = (uint) optIndexCount;
    mMeshes[meshIndex].numVertices = (uint) optVertexCount;
}

void AssimpLoader::loadTextures(uint materialIndex) {
    for (uint texType = 0; texType < ModelTexture::UNKNOWN; ++texType ) {
        loadTexture((ModelTexture::Type) texType, materialIndex);
    }
}

void AssimpLoader::loadTexture(ModelTexture::Type textureType, uint materialIndex) {
    const aiMaterial* material = mScene->mMaterials[materialIndex];
    aiTextureType assimpType = ModelTexture::toAssimpType(textureType);
    ModelTexture texture;
    aiString path;
    if (material->GetTextureCount(assimpType) <= 0) {
       texture.path = PROJECT_PATH"textures/no_texture.jpeg";
    } else if (material->GetTexture(assimpType, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
        const aiTexture* aiTex = mScene->GetEmbeddedTexture(path.C_Str());
        if (aiTex) {
            texture.embedded = true;
            texture.bufferSize = aiTex->mWidth; //???
            texture.data = aiTex->pcData;
        } else {
            fs::path modelPath(mModelPath);
            std::string dirPath = modelPath.parent_path();
            texture.path = dirPath + path.C_Str();
        }
    }
    mMaterials[materialIndex].mTextures[textureType] = texture;
}

void AssimpLoader::loadColors(uint materialIndex) {
    ModelMaterial& material = mMaterials[materialIndex];
    const aiMaterial* aiMat = mScene->mMaterials[materialIndex];

    material.mName = aiMat->GetName().C_Str();

    aiColor4D ambientColor(0);
    if (aiMat->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == AI_SUCCESS) {
        material.ambientColor.r = ambientColor.r;
        material.ambientColor.g = ambientColor.g;
        material.ambientColor.b = ambientColor.b;
        material.ambientColor.a = std::min(ambientColor.a, 1.0f);
    }
    aiColor4D emissiveColor(0);
    if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == AI_SUCCESS) {
        material.ambientColor.r += emissiveColor.r;
        material.ambientColor.g += emissiveColor.g;
        material.ambientColor.b += emissiveColor.b;
        material.ambientColor.a += emissiveColor.a;
        material.ambientColor.a = std::min(material.ambientColor.a, 1.0f);
    }

    aiColor4D diffuseColor(0);
    if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS) {
        material.diffuseColor.r = diffuseColor.r;
        material.diffuseColor.g = diffuseColor.g;
        material.diffuseColor.b = diffuseColor.b;
        material.diffuseColor.a = std::min(diffuseColor.a, 1.0f);
    }

    aiColor4D specularColor(0.0f, 0.0f, 0.0f, 0.0f);
    if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == AI_SUCCESS) {
        material.specularColor.r = specularColor.r;
        material.specularColor.g = specularColor.g;
        material.specularColor.b = specularColor.b;
        material.specularColor.a = std::min(specularColor.a, 1.0f);
    }

    float opaquenessThreshold = 0.05f;
    float opacity = 1.0f;

    if (aiMat->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
        material.mTransparencyFactor = std::clamp(1.0f - opacity, 0.0f, 1.0f);
        if (material.mTransparencyFactor >= 1.0f - opaquenessThreshold) {
            material.mTransparencyFactor = 0.0f;
        }
    }

    aiColor4D TransparentColor;
    if (aiMat->Get(AI_MATKEY_COLOR_TRANSPARENT, TransparentColor) == AI_SUCCESS) {
        float Opacity = std::max(std::max(TransparentColor.r, TransparentColor.g), TransparentColor.b);
        material.mTransparencyFactor = std::clamp(Opacity, 0.0f, 1.0f);
        if (material.mTransparencyFactor >= 1.0f - opaquenessThreshold) {
            material.mTransparencyFactor = 0.0f;
        }

        material.mAlphaTest = 0.5f;
    }
}
