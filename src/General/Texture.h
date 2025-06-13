//
// Created by auser on 6/12/25.
//

#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H

#include "Context.h"

class Texture {
public:
    Texture(Context* context);
    Texture(Context* context, bool generateMipMap);
    ~Texture();

    VkImage image() { return mImage; }
    VkImageView imageView() { return mImageView; }
    VkSampler sampler() { return mSampler; }

    void allocate(int width, int height);
    void destroy();
    void load(void* data, int bufferSize);
    void load(const std::string& path);
    void transit(VkImageLayout src, VkImageLayout dst);

    static int calcNumMipMaps(int width, int height);
private:
    void generateMipMaps(VkFormat imageFormat);
    void load(void* data);

    Context* mContext;
    VkImage mImage;
    VkImageView mImageView;
    VmaAllocation mImageAllocation;
    VkSampler mSampler;
    int mMipLevels;
    int mTexWidth, mTexHeight, mTexChannels;
    bool mGenerateMipMap;
};


#endif //VULKAN_TEXTURE_H
