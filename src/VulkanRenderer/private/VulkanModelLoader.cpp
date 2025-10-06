//
// Created by auser on 6/12/25.
//

#include "VulkanModelLoader.hpp"
#include "Message.hpp"
#include "AbsLoader.hpp"

#include <utility>

VulkanModelLoader::VulkanModelLoader(Context* context, std::string modelPath, const std::vector<std::string>& skyBoxPaths):
                                     Loader(std::move(modelPath)), mContext(context) {
    mSkyBox = std::make_unique<CubeMapImage>(context);
    mSkyBox->load({skyBoxPaths});
    CubeMapTransitInfo transitInfo{
        .src = VK_IMAGE_LAYOUT_UNDEFINED,
        .dst = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .layer = 0,
        .layerCount = 6,
    };
    mSkyBox->transit(transitInfo);
}
VulkanModelLoader::VulkanModelLoader(Context* context, std::string modelPath): Loader(std::move(modelPath)),
                                                                               mContext(context) {
}

VulkanModelLoader::~VulkanModelLoader() {
    for (auto& vulkanTextureVec: mVulkanTextures) {
        for (auto& mTexture : vulkanTextureVec.mTextures){
            mTexture->destroy();
            delete mTexture;
            mTexture = nullptr;
        }
    }
    if (mSkyBox) mSkyBox->destroy();
}

bool VulkanModelLoader::load() {
    return Loader::load() &&
           loadGeometry()       &&
           loadMaterials();
}

bool VulkanModelLoader::loadGeometry() {
    const std::vector<cm::Mesh>& meshes = mLoader->meshes();
    const std::vector<cm::Vertex>& vertices = mLoader->vertices();
    mVulkanVertices.reserve(vertices.size());
    for (size_t i = 0; i < meshes.size(); ++i) {
        const cm::Mesh& mesh = meshes[i];
        for (size_t j = 0; j < mesh.numVertices; ++j) {
            const cm::Vertex& modelVertex = vertices[mesh.baseVertex + j];
            Vertex vert{
                    .pos = modelVertex.pos,
                    .color = modelVertex.color,
                    .texCoord = modelVertex.texCoord0,
                    .normal = modelVertex.normal,
                    .tangent = modelVertex.tangent,
                    .texIndex = (uint32_t) mesh.materialIndex * cm::Texture::UNKNOWN
            };
            mVulkanVertices.push_back(vert);
        }
    }
    return true;
}

bool VulkanModelLoader::loadMaterials() {
    const std::vector<cm::Material>& materials = mLoader->materials();
    mVulkanTextures.resize(materials.size());
    for (size_t i = 0; i < materials.size(); ++i) {
        const cm::Material& material = materials[i];
        VulkanTextures& vulkanTextures = mVulkanTextures[i];
        for (int tex = 0; tex < (int) cm::Texture::UNKNOWN; ++tex) {
            const cm::Texture& texture = material.mTextures[tex];
            Texture*& vulkanTexture = vulkanTextures[tex];
            vulkanTexture = new Texture(mContext);
            TextureLoadInfo loadInfo{
                .data = texture.empty() ? getEmptyData((cm::Texture::Type) tex) : texture.data,
                .width = static_cast<uint32_t>(texture.empty() ? 1 : texture.width),
                .height = static_cast<uint32_t>(texture.empty() ? 1 : texture.height),
                .path = texture.path,
                .texType = (cm::Texture::Type) tex,
                .generateMipMap = !texture.empty()
            };
            vulkanTexture->load(loadInfo);
        }
    }
    return true;
}

static glm::vec3 toEmptyColor(cm::Texture::Type texType) {
    switch(texType) {
        case cm::Texture::Type::DIFFUSE:
            return {1, 0, 0};
        case cm::Texture::Type::SPECULAR:
        case cm::Texture::Type::SHININESS:
        case cm::Texture::Type::AMBIENT:
            return glm::vec3(0);
        case cm::Texture::Type::NORMAL:
            return {0, 1, 0};
        default:
            INFO << "ID: " << texType;
            throw std::runtime_error("Unsupported model texture type!");
    }
}

void* VulkanModelLoader::getEmptyData(cm::Texture::Type texType) {
    gli::texture2d tex(gli::FORMAT_RGBA8_UNORM_PACK8, {1,1}, 1);
    tex.clear(gli::packUnorm4x8(glm::vec4(toEmptyColor(texType), 1)));
    return tex.data();
}