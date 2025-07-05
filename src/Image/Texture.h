//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H

#include "Context.h"
#include "ModelMaterial.h"
#include "SampledImage.h"

#include <gli/gli.hpp>

struct TextureAllocateInfo{
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkExtent2D extent = {0, 0};
    VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
    VkImageUsageFlags imageUsageFlags = 0;
    VkImageAspectFlags aspectFlags = 0;
    uint32_t mipLevels = 1;
    bool generateMipMaps = false;
};

struct TextureLoadInfo{
    void* data = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    std::string path;
    ModelTexture::Type texType = ModelTexture::Type::UNKNOWN;
    bool generateMipMap = false;
};

class Texture: public SampledImage {
public:
    Texture(Context* context);
    ~Texture();

    void load(TextureLoadInfo& loadInfo);

    void allocate(TextureAllocateInfo& allocateInfo);

    static VkFormat toVkFormat(ModelTexture::Type modelTexType);
    static VkFormat toVkFormat(gli::texture::format_type gliFormat);

    void transit(ImageTransitInfoCmd& transitInfo) override;
    void transit(ImageTransitInfo& transitInfo) override;
protected:
    TextureAllocateInfo getAllocateInfo() const;
    TextureAllocateInfo getAllocateInfo(TextureLoadInfo& loadInfo) const;
    TextureAllocateInfo getAllocateInfo(const gli::texture& tex) const;

    void loadByData(TextureLoadInfo& loadInfo);
    void loadByPath(TextureLoadInfo& loadInfo);
    void load(void* data, VkExtent2D extent, int mipLevel);
    bool loadCommon(const std::string& path);
    bool loadCompressed(const std::string& path);
    void generateMipMaps();
// deleted
    void allocate(ImageAllocateInfo& allocateInfo);

    uint32_t mTexChannels;
    uint32_t mMipLevels;
    bool mGenerateMipMap;
};


#endif //VULKAN_TEXTURE_H
