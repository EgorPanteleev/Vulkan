//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_MODELMATERIAL_H
#define VULKAN_MODELMATERIAL_H

#include <string>
#include <array>
#include <map>
#include <assimp/material.h>
#include <glm/glm.hpp>

struct ModelTexture {
    enum Type {
        DIFFUSE = 0,
        NORMAL  = 1,
        UNKNOWN = 2
    };

    ModelTexture(): path(), data(nullptr), bufferSize(0), embedded(false) {}

    static aiTextureType toAssimpType(Type type);

    std::string path;
    void* data;
    uint32_t bufferSize;
    bool embedded;
};

static std::map<ModelTexture::Type, aiTextureType> toAssimpTypeMap {
        { ModelTexture::DIFFUSE, aiTextureType_DIFFUSE },
        { ModelTexture::NORMAL , aiTextureType_NORMALS },
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
