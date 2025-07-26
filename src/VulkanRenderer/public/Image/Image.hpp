//
// Created by igor on 6/30/25.
//

#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include "Context.hpp"

struct ImageAllocateInfo{
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkExtent2D extent = {0, 0};
    VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
    VkImageUsageFlags imageUsageFlags = 0;
    VkImageAspectFlags aspectFlags = 0;
};

struct ImageTransitInfoCmd {
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkImageLayout src = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout dst = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t level = 0;
    uint32_t levelCount = 0;
};

struct ImageTransitInfo {
    VkImageLayout src = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout dst = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t level = 0;
    uint32_t levelCount = 0;
};

class Image {
public:
    Image(Context* context);
    virtual ~Image();

    VkImage image() { return mImage; }
    VkImageView imageView() { return mImageView; }
    VkExtent2D extent() { return mExtent; }

    void allocate(ImageAllocateInfo& allocateInfo);
    void destroy();

    static uint32_t calcMipLevels(uint32_t width, uint32_t height);

    virtual void transit(ImageTransitInfoCmd& transitInfo);
    virtual void transit(ImageTransitInfo& transitInfo);

    void set(VkImage image, VkImageView imageView) {mImage = image; mImageView = imageView;};

protected:

    Context* mContext;
    VkImage mImage;
    VkImageView mImageView;
    VmaAllocation mImageAllocation;

    VkFormat mFormat;
    VkExtent2D mExtent;
};


#endif //VULKAN_IMAGE_H
