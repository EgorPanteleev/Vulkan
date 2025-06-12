//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_MODELMATERIAL_H
#define VULKAN_MODELMATERIAL_H
/*TODO
1) create texture class, which handles vulkan textures
2) load textures as i know
3) change graphics pipeline to create descriptor set per mesh
*/
#include <string>
#include <array>
#include <map>
#include <assimp/material.h>
#include <glm/glm.hpp>

struct ModelTexture {
    enum Type {
        DIFFUSE = 0,
        UNKNOWN = 1
    };

    ModelTexture(): path(), data(nullptr), bufferSize(0), embedded(false) {}

    static aiTextureType toAssimpType(Type type);

    std::string path;
    void* data;
    u_int32_t bufferSize;
    bool embedded;
};

static std::map<ModelTexture::Type, aiTextureType> toAssimpTypeMap {
        { ModelTexture::DIFFUSE, aiTextureType_DIFFUSE },
        { ModelTexture::UNKNOWN, aiTextureType_UNKNOWN }
};

struct ModelMaterial {
    ModelMaterial(): mName(), ambientColor(0), diffuseColor(0), specularColor(0),
                     mTransparencyFactor(1), mAlphaTest(0) {}

    std::string mName;

    glm::vec4 ambientColor;
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;

    float mTransparencyFactor;
    float mAlphaTest;

    std::array<ModelTexture, ModelTexture::UNKNOWN> mTextures;
};

#endif //VULKAN_MODELMATERIAL_H
