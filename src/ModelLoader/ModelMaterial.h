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
        DIFFUSE   = 0,
        SPECULAR  = 1,
        SHININESS = 2,
        AMBIENT   = 3,
        NORMAL    = 4,
        UNKNOWN   = 5
    };

    ModelTexture(): path(), data(nullptr), width(0), height(0), embedded(false) {}

    bool empty() { return path.empty() && !data; }

    static aiTextureType toAssimpType(Type type);

    std::string path;
    void* data;
    int width;
    int height;
    bool embedded;
};

static std::map<ModelTexture::Type, aiTextureType> toAssimpTypeMap {
        { ModelTexture::DIFFUSE  , aiTextureType_DIFFUSE   },
        { ModelTexture::SPECULAR , aiTextureType_SPECULAR  },
        { ModelTexture::SHININESS, aiTextureType_SHININESS },
        { ModelTexture::AMBIENT  , aiTextureType_AMBIENT   },
        { ModelTexture::NORMAL   , aiTextureType_NORMALS   },
        { ModelTexture::UNKNOWN  , aiTextureType_UNKNOWN   }
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
