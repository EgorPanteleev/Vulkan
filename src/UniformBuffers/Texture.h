//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H

#include "Context.h"
#include "ModelMaterial.h"

#include <gli/gli.hpp>

struct TextureLoadInfo{
    void* data = nullptr;
    int width = 0;
    int height = 0;
    std::string path;
    ModelTexture::Type texType = ModelTexture::Type::UNKNOWN;
    bool generateMipMap = false;
};

class Texture {
public:
    Texture(Context* context);
    ~Texture();

    VkImage image() { return mImage; }
    VkImageView imageView() { return mImageView; }
    VkSampler sampler() { return mSampler; }

    void allocate();
    void destroy();
    void load(TextureLoadInfo& loadInfo);
    void transit(VkImageLayout src, VkImageLayout dst);

    static int calcNumMipMaps(int width, int height);
    static VkFormat toVkFormat(ModelTexture::Type modelTexType);
    static VkFormat toVkFormat(gli::texture::format_type gliFormat);
private:
    void generateMipMaps();
    void loadByData(TextureLoadInfo& loadInfo);
    void loadByPath(TextureLoadInfo& loadInfo);
    void load(void* data, VkExtent2D extent, int mipLevel);
    bool loadCommon(const std::string& path);
    bool loadCompressed(const std::string& path);

    Context* mContext;
    VkImage mImage;
    VkImageView mImageView;
    VmaAllocation mImageAllocation;
    VkSampler mSampler;
    VkFormat mFormat;

    int mMipLevels;
    int mTexWidth, mTexHeight, mTexChannels;
    bool mGenerateMipMap;
};


#endif //VULKAN_TEXTURE_H
