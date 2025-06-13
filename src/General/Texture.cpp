//
// Created by auser on 6/12/25.
//

#include "Texture.h"
#include "Utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(Context* context): mContext(context), mMipLevels(1), mGenerateMipMap(false) {
}

Texture::Texture(Context* context, bool generateMipMap): mContext(context), mMipLevels(1), mGenerateMipMap(generateMipMap) {
}

Texture::~Texture() {
    vkDestroySampler(mContext->device(), mSampler, nullptr);
    vkDestroyImageView(mContext->device(), mImageView, nullptr);
    vmaDestroyImage(mContext->allocator(), mImage, mImageAllocation);
}

void Texture::allocate(int width, int height) {
    if (mGenerateMipMap) mMipLevels = calcNumMipMaps(width, height);
    Utils::createImage(mContext->allocator(), mImageAllocation, VMA_MEMORY_USAGE_AUTO,
                       mImage, mMipLevels, VK_SAMPLE_COUNT_1_BIT,
                       mTexWidth, mTexHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                       VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT );


    transit(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    mImageView = Utils::createImageView(mContext->device(), mImage, mMipLevels, VK_IMAGE_VIEW_TYPE_2D,
                                        VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

    Utils::createSampler(mContext, mSampler, mMipLevels,
                         VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_FALSE);
}

void Texture::load(void* data, int bufferSize) {
    void* imageData = stbi_load_from_memory((const stbi_uc*)data, bufferSize,
                                            &mTexWidth, &mTexHeight, &mTexChannels, 0);
    load(imageData);
}

void Texture::load(const std::string& path) {
    stbi_uc* pixels = stbi_load(path.c_str(), &mTexWidth, &mTexHeight, &mTexChannels, STBI_rgb_alpha);
    load((void*) pixels);
}

void Texture::load(void* data) {
    allocate(mTexWidth, mTexHeight);
    if (!data) {
        throw std::runtime_error("Failed to load texture image!");
    }
    VkDeviceSize imageSize = mTexWidth * mTexHeight * 4;
    VkBuffer stagingBuffer;
    VmaAllocation allocation;

    Utils::createBuffer(mContext->allocator(), allocation, VMA_MEMORY_USAGE_CPU_ONLY, imageSize,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);

    Utils::copyDataToBuffer( mContext->allocator(), allocation, data, static_cast<size_t>(imageSize));
    Utils::copyBufferToImage(mContext, stagingBuffer, mImage,
                             static_cast<uint32_t>(mTexWidth), static_cast<uint32_t>(mTexHeight));
    if (!mGenerateMipMap)
        transit(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vmaDestroyBuffer(mContext->allocator(), stagingBuffer, allocation);
    generateMipMaps(VK_FORMAT_R8G8B8A8_SRGB);
}

void Texture::transit(VkImageLayout src, VkImageLayout dst) {
    Utils::transitionImageLayout(mContext, mImage, mMipLevels, VK_FORMAT_R8G8B8A8_SRGB, src, dst);
}

int Texture::calcNumMipMaps(int width, int height) {
    return std::floor(std::log2(std::max(width, height))) + 1;
}

void Texture::generateMipMaps(VkFormat imageFormat) {
    if (!mGenerateMipMap) return;
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(mContext->physicalDevice(), imageFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("Texture image format does not support linear blitting!");
    }

    auto commandPool = Utils::createCommandPool(mContext, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT );
    VkCommandBuffer commandBuffer = Utils::beginSingleTimeCommands(mContext->device(), commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = mImage;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    auto mipWidth = (int32_t) mTexWidth;
    auto mipHeight = (int32_t) mTexHeight;

    for (uint32_t i = 1; i < mMipLevels; ++i) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        vkCmdBlitImage(commandBuffer,
                       mImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mMipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);
    Utils::endSingleTimeCommands(mContext, commandPool, commandBuffer);
    vkDestroyCommandPool(mContext->device(), commandPool, nullptr);
}
