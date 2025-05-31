//
// Created by auser on 4/7/25.
//

#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include "Context.h"

//Texture Image
class Image {
public:
    Image(Context* context, const std::string& path );
    ~Image();

    VkImage image() { return mImage; }
    VmaAllocation imageAllocation() { return mImageAllocation; }
    VkImageView imageView() { return mImageView; }
    VkSampler sampler() { return mSampler; }
private:
    void createImage(const std::string& path);
    void createImageView();
    void generateMipMaps(VkFormat imageFormat);

    Context* mContext;
    VkImage mImage;
    uint32_t mMipLevels;
    uint32_t mTexWidth, mTexHeight, mTexChannels;
    VmaAllocation mImageAllocation;
    VkImageView mImageView;
    VkSampler mSampler;
};


#endif //VULKAN_IMAGE_H
