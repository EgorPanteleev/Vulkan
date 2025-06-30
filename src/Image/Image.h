//
// Created by igor on 6/30/25.
//

#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include "Context.h"

struct ImageAllocateInfo{
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkExtent2D extent = {0, 0};
    VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
    VkImageUsageFlags imageUsageFlags = 0;
    VkImageAspectFlags aspectFlags = 0;
    uint32_t mipLevels = 1;
    bool generateMipMaps = false;
};

class Image {
public:
    Image(Context* context);
    virtual ~Image();

    VkImage image() { return mImage; }
    VkImageView imageView() { return mImageView; }
    VkSampler sampler() { return mSampler; }
    VkExtent2D extent() { return mExtent; }

    void allocate(ImageAllocateInfo& allocateInfo);
    void destroy();

    void transit(VkCommandBuffer commandBuffer, VkImageLayout src, VkImageLayout dst,
                 uint32_t level = 0, uint32_t levelCount = 0);
    void transit(VkImageLayout src, VkImageLayout dst, uint32_t level = 0, uint32_t levelCount = 0);

    static uint32_t calcMipLevels(uint32_t width, uint32_t height);
protected:
    void generateMipMaps();

    Context* mContext;
    VkImage mImage;
    VkImageView mImageView;
    VmaAllocation mImageAllocation;
    VkSampler mSampler;

    VkFormat mFormat;
    VkExtent2D mExtent;
    uint32_t mMipLevels;
    bool mGenerateMipMap;
};


#endif //VULKAN_IMAGE_H
