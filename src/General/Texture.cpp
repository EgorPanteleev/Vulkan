//
// Created by auser on 6/12/25.
//

#include "Texture.h"
#include "Utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <gli/gli.hpp>

Texture::Texture(Context* context): mContext(context), mMipLevels(1), mGenerateMipMap(false) {
}

Texture::Texture(Context* context, bool generateMipMap): mContext(context), mMipLevels(1), mGenerateMipMap(generateMipMap) {
}

Texture::~Texture() {
    destroy();
}

void Texture::allocate() {
    Utils::createImage(mContext->allocator(), mImageAllocation, VMA_MEMORY_USAGE_AUTO,
                       mImage, mMipLevels, VK_SAMPLE_COUNT_1_BIT,
                       mTexWidth, mTexHeight, mFormat, VK_IMAGE_TILING_OPTIMAL,
                       VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT );


    transit(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    mImageView = Utils::createImageView(mContext->device(), mImage, mMipLevels, VK_IMAGE_VIEW_TYPE_2D,
                                        mFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    Utils::createSampler(mContext, mSampler, mMipLevels,
                         VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_FALSE);
}

void Texture::destroy() {
    vkDestroySampler(mContext->device(), mSampler, nullptr);
    vkDestroyImageView(mContext->device(), mImageView, nullptr);
    vmaDestroyImage(mContext->allocator(), mImage, mImageAllocation);
}

void Texture::load(void* data, int bufferSize) {
    void* imageData = stbi_load_from_memory((const stbi_uc*)data, bufferSize,
                                            &mTexWidth, &mTexHeight, &mTexChannels, 0);
    allocate();
    load(imageData, {(uint32_t)mTexWidth, (uint32_t)mTexHeight}, 0);
}

static VkFormat gliToVulkanFormat(gli::texture::format_type gliFormat) {
    VkFormat res;
    switch(gliFormat) {
        case gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8:
            res = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
            break;
        case gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16:
            res = VK_FORMAT_BC3_UNORM_BLOCK;
            break;
        default:
            res = VK_FORMAT_R8G8B8A8_SRGB;
            break;
    }
    return res;
}

static int formatToSize(VkFormat format, VkExtent2D extent) {
    int res;
    switch(format) {
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
            res = ((extent.width + 3) / 4) * ((extent.height + 3) / 4) * 8;;
            break;
        case VK_FORMAT_BC3_UNORM_BLOCK:
            res = ((extent.width + 3) / 4) * ((extent.height + 3) / 4) * 16;;
            break;
        default:
            res = extent.width * extent.height * 4;
            break;
    }
    return res;
}

void Texture::load(const std::string& path) {
    bool status = loadCommon(path);
    if (!status) status = loadCompressed(path);
    if (!status) throw std::runtime_error("Failed to load texture!");
}

bool Texture::loadCommon(const std::string& path) {
    void* pixels = stbi_load(path.c_str(), &mTexWidth, &mTexHeight, &mTexChannels, STBI_rgb_alpha);
    if (!pixels) return false;
    mFormat = VK_FORMAT_R8G8B8A8_SRGB;
    if (mGenerateMipMap) mMipLevels = calcNumMipMaps(mTexWidth, mTexHeight);
    allocate();
    load(pixels, {(uint32_t)mTexWidth, (uint32_t)mTexHeight}, 0);
    return true;
}

bool Texture::loadCompressed(const std::string& path) {
    gli::texture tex = gli::load(path.c_str());
    if (tex.empty()) return false;
    mMipLevels = (int) tex.levels();
    mGenerateMipMap = false; // Compressed images doesnt support generating mipMaps
    int layer = 0; int face = 0;
    mFormat = gliToVulkanFormat(tex.format());
    mTexWidth = tex.extent().x;
    mTexHeight = tex.extent().y;
    allocate();
    for (int level = 0; level < tex.levels(); ++level) {
        void* pixels = tex.data(layer, face, level);
        VkExtent2D extent = { (uint32_t)tex.extent(level).x, (uint32_t)tex.extent(level).y };
        load(pixels, extent, level);
    }
    return true;
}


void Texture::load(void* data, VkExtent2D extent, int mipLevel) {
    VkDeviceSize imageSize = formatToSize(mFormat, extent);
    VkBuffer stagingBuffer;
    VmaAllocation allocation;

    Utils::createBuffer(mContext->allocator(), allocation, VMA_MEMORY_USAGE_CPU_ONLY, imageSize,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);

    Utils::copyDataToBuffer(mContext->allocator(), allocation, data, (size_t)imageSize);
    Utils::copyBufferToImage(mContext, stagingBuffer, mImage,
                             extent.width, extent.height, mipLevel);

    vmaDestroyBuffer(mContext->allocator(), stagingBuffer, allocation);
    if (mGenerateMipMap) generateMipMaps();
}

void Texture::transit(VkImageLayout src, VkImageLayout dst) {
    Utils::transitionImageLayout(mContext, mImage, mMipLevels, VK_FORMAT_R8G8B8A8_SRGB, src, dst);
}

int Texture::calcNumMipMaps(int width, int height) {
    return std::floor(std::log2(std::max(width, height))) + 1;
}

void Texture::generateMipMaps() {
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(mContext->physicalDevice(), mFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        WARNING << "WARNING: Texture image format does not support linear blitting!";
        return;
    }
    auto commandPool = Utils::createCommandPool(mContext, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT );
    VkCommandBuffer commandBuffer = Utils::beginSingleTimeCommands(mContext->device(), commandPool);

    VkImageMemoryBarrier barrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = mImage,
            .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
            },
    };

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

        VkImageBlit blit{
                .srcSubresource = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = i - 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                },
                .srcOffsets = {
                        { 0, 0, 0 },
                        { mipWidth, mipHeight, 1 },
                },
                .dstSubresource = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = i,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                },
                .dstOffsets = {
                        { 0, 0, 0 },
                        { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 },
                },
        };
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
