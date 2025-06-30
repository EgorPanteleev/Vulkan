//
// Created by igor on 6/30/25.
//

#include "Image.h"
#include "Utils.h"

Image::Image(Context* context): mContext(context), mMipLevels(1), mGenerateMipMap(false) {
}

Image::~Image() {
}

void Image::allocate(ImageAllocateInfo& allocateInfo) {
    mFormat = allocateInfo.format;
    mExtent = allocateInfo.extent;
    mMipLevels = allocateInfo.mipLevels;
    mGenerateMipMap = allocateInfo.generateMipMaps;

    Utils::createImage(mContext->allocator(), mImageAllocation, VMA_MEMORY_USAGE_AUTO,
                       mImage, mMipLevels, allocateInfo.numSamples,
                       mExtent.width, mExtent.height, mFormat, VK_IMAGE_TILING_OPTIMAL,
                       allocateInfo.imageUsageFlags);

    mImageView = Utils::createImageView(mContext->device(), mImage, mMipLevels, VK_IMAGE_VIEW_TYPE_2D,
                                        mFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    Utils::createSampler(mContext, mSampler, mMipLevels,
                         VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_FALSE);
}

void Image::destroy() {
    vkDestroySampler(mContext->device(), mSampler, nullptr);
    vkDestroyImageView(mContext->device(), mImageView, nullptr);
    vmaDestroyImage(mContext->allocator(), mImage, mImageAllocation);
}

void Image::transit(VkCommandBuffer commandBuffer, VkImageLayout src, VkImageLayout dst,
                    uint32_t level, uint32_t levelCount) {
    Utils::transitionImageLayout(commandBuffer,mImage,
                                 mMipLevels,mFormat,
                                 src,dst, level, levelCount);
}

void Image::transit(VkImageLayout src, VkImageLayout dst, uint32_t level, uint32_t levelCount) {
    Utils::transitionImageLayout(mContext,mImage,
                                 mMipLevels,mFormat,
                                 src,dst, level, levelCount);
}

uint32_t Image::calcMipLevels(uint32_t width, uint32_t height) {
    return std::floor(std::log2(std::max(width, height))) + 1;
}

void Image::generateMipMaps() {
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(mContext->physicalDevice(), mFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        WARNING << "WARNING: Texture image format does not support linear blitting!";
        return;
    }

    auto commandPool = Utils::createCommandPool(mContext, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    VkCommandBuffer commandBuffer = Utils::beginSingleTimeCommands(mContext->device(), commandPool);

    int32_t mipWidth = static_cast<int32_t>(mExtent.width);
    int32_t mipHeight = static_cast<int32_t>(mExtent.height);

    for (uint32_t i = 1; i < mMipLevels; ++i) {
        // 1. Transition (i-1) level from DST to SRC
        transit(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, i - 1, 1);

        // 2. Blit to mip i
        VkImageBlit blit{
                .srcSubresource = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = i - 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                },
                .srcOffsets = {
                        {0, 0, 0},
                        {mipWidth, mipHeight, 1},
                },
                .dstSubresource = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = i,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                },
                .dstOffsets = {
                        {0, 0, 0},
                        {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1},
                },
        };

        vkCmdBlitImage(commandBuffer,
                       mImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);

        // 3. Transition (i-1) to SHADER_READ layout
        transit(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i - 1, 1);

        // 4. Prepare for next loop
        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    // Последний уровень в TRANSFER_DST → SHADER_READ
    transit(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mMipLevels - 1, 1);

    Utils::endSingleTimeCommands(mContext, commandPool, commandBuffer);
    vkDestroyCommandPool(mContext->device(), commandPool, nullptr);
}