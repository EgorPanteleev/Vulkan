//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H

#include "Context.h"
#include "ModelMaterial.h"
#include "Image.h"

#include <gli/gli.hpp>

struct TextureLoadInfo{
    void* data = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    std::string path;
    ModelTexture::Type texType = ModelTexture::Type::UNKNOWN;
    bool generateMipMap = false;
};

class Texture: public Image {
public:
    Texture(Context* context);
    ~Texture();

    void load(TextureLoadInfo& loadInfo);

    static VkFormat toVkFormat(ModelTexture::Type modelTexType);
    static VkFormat toVkFormat(gli::texture::format_type gliFormat);
private:
    ImageAllocateInfo getAllocateInfo() const;
    ImageAllocateInfo getAllocateInfo(TextureLoadInfo& loadInfo) const;
    ImageAllocateInfo getAllocateInfo(const gli::texture& tex) const;

    void loadByData(TextureLoadInfo& loadInfo);
    void loadByPath(TextureLoadInfo& loadInfo);
    void load(void* data, VkExtent2D extent, int mipLevel);
    bool loadCommon(const std::string& path);
    bool loadCompressed(const std::string& path);

    uint32_t mTexChannels;
};


#endif //VULKAN_TEXTURE_H
