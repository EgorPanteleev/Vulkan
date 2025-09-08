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
};

struct ImageTransitInfo {
    VkImageLayout src = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout dst = VK_IMAGE_LAYOUT_UNDEFINED;
};

struct ImageCreateInfo {
    VmaAllocator allocator;
    VmaAllocation& imageAllocation;
    VmaMemoryUsage allocUsage;
    VkImage& image;
    VkImageCreateFlags flags;
    uint32_t mipLevels;
    VkSampleCountFlagBits numSamples;
    uint32_t width;
    uint32_t height;
    uint32_t arrayLayers;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags imageUsage;
};

struct ImageViewCreateInfo {
    VkDevice device = VK_NULL_HANDLE;
    VkImage image = VK_NULL_HANDLE;
    uint32_t layerCount = 1;
    uint32_t mipLevels = 0;
    VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_NONE;
};

struct ImageAndViewCreateInfo{
    Context* context;
    VmaAllocation& imageAllocation;
    VkImage& image;
    VkImageCreateFlags flags;
    VkImageView& imageView;
    VkImageViewType viewType;
    uint32_t mipLevels;
    VkSampleCountFlagBits numSamples;
    VkExtent2D extent;
    uint32_t arrayLayers;
    VkFormat format;
    VkImageUsageFlags imageUsageFlags;
    VkImageAspectFlags aspectFlags;
};

struct SamplerCreateInfo{
    Context* context;
    VkSampler& sampler;
    uint32_t mipLevels;
    VkSamplerAddressMode adressMode;
    VkBorderColor borderColor;
    VkBool32 compare;
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

    void transit(ImageTransitInfoCmd& transitInfo);
    void transit(ImageTransitInfo& transitInfo);

    void set(VkImage image, VkImageView imageView) {mImage = image; mImageView = imageView;};

    static void createImage(const ImageCreateInfo& createInfo);
    static VkImageView createImageView(const ImageViewCreateInfo& createInfo);
    static void createImageAndView(const ImageAndViewCreateInfo& createInfo);
    static void createSampler(const SamplerCreateInfo& createInfo);

protected:

    Context* mContext;
    VkImage mImage;
    VkImageView mImageView;
    VmaAllocation mImageAllocation;

    VkFormat mFormat;
    VkExtent2D mExtent;
};


#endif //VULKAN_IMAGE_H
