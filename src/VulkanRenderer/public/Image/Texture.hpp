//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H

#include "Context.hpp"
#include "Material.hpp"
#include "SampledImage.hpp"

#include <gli/gli.hpp>

namespace cm = crv::model;

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
    cm::Texture::Type texType = cm::Texture::Type::UNKNOWN;
    bool generateMipMap = false;
};

struct TextureTransitInfoCmd {
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkImageLayout src = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout dst = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t level = 0;
    uint32_t levelCount = 0;
};

struct TextureTransitInfo {
    VkImageLayout src = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout dst = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t level = 0;
    uint32_t levelCount = 0;
};

class Texture: public SampledImage {
public:
    Texture(Context* context);
    ~Texture();

    void load(TextureLoadInfo& loadInfo);

    void allocate(TextureAllocateInfo& allocateInfo);

    static VkFormat toVkFormat(cm::Texture::Type modelTexType);
    static VkFormat toVkFormat(gli::texture::format_type gliFormat);

    void transit(TextureTransitInfoCmd& transitInfo);
    void transit(TextureTransitInfo& transitInfo);
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
