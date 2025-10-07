//
// Created by auser on 6/12/25.
//

#include "Texture.hpp"
#include "Utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(Context* context): SampledImage(context), mTexChannels(4), mMipLevels(1), mGenerateMipMap(false) {
}

Texture::~Texture() {
}

static VkFormat toVkFormat( cm::Texture::Format format ) {
    switch(format) {
        case cm::Texture::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case cm::Texture::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case cm::Texture::BC1_UNORM:
            return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case cm::Texture::BC3_UNORM:
            return VK_FORMAT_BC3_UNORM_BLOCK;
        case cm::Texture::BC5_UNORM:
            return VK_FORMAT_BC5_UNORM_BLOCK;
        default:
            throw std::runtime_error("Unsupported texture format!");
    }
}

void Texture::load(TextureLoadInfo& loadInfo) {
    if ( loadInfo.dataByLevel.empty() ) return;
    if ( loadInfo.dataByLevel.size() > 1 ) {
        mGenerateMipMap = false;
        mMipLevels = loadInfo.dataByLevel.size();
    }
    else mGenerateMipMap = loadInfo.generateMipMap;
    mFormat = toVkFormat(loadInfo.texFormat);
    mExtent = {  loadInfo.dataByLevel[0].width, loadInfo.dataByLevel[0].height };
    if (mGenerateMipMap) mMipLevels = calcMipLevels(mExtent.width, mExtent.height);
    TextureAllocateInfo allocateInfo = getAllocateInfo();
    allocate(allocateInfo);
    for ( int level = 0; level < loadInfo.dataByLevel.size(); ++level ) {
        cm::Texture::LevelData levelData = loadInfo.dataByLevel[level];
        load(levelData.data, { levelData.width, levelData.height }, level);
    }
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

void Texture::load(void* data, VkExtent2D extent, int mipLevel) {
    TextureTransitInfo transitInfo{
      .src = VK_IMAGE_LAYOUT_UNDEFINED,
      .dst = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    };
    transit(transitInfo);
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

void Texture::allocate(TextureAllocateInfo& allocateInfo){
    mFormat = allocateInfo.format;
    mExtent = allocateInfo.extent;
    mMipLevels = allocateInfo.mipLevels;
    mGenerateMipMap = allocateInfo.generateMipMaps;
    ImageAndViewCreateInfo imageAndViewCreateInfo{
            .context = mContext,
            .imageAllocation = mImageAllocation,
            .image = mImage,
            .imageView = mImageView,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .mipLevels = mMipLevels,
            .numSamples = allocateInfo.numSamples,
            .extent = mExtent,
            .arrayLayers = 1,
            .format = mFormat,
            .imageUsageFlags = allocateInfo.imageUsageFlags,
            .aspectFlags = allocateInfo.aspectFlags
    };
    createImageAndView(imageAndViewCreateInfo);
    SamplerCreateInfo samplerCreateInfo{
            .context = mContext,
            .sampler = mSampler,
            .mipLevels = mMipLevels,
            .adressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .compare = VK_FALSE,
    };
    createSampler(samplerCreateInfo);
}

void Texture::transit(TextureTransitInfoCmd& transitInfo) {
    Utils::transitionImageLayout(transitInfo.commandBuffer, mImage,
                                 mMipLevels, mFormat,
                                 transitInfo.src, transitInfo.dst,
                                 transitInfo.level, transitInfo.levelCount,
                                 0, 1);
}

void Texture::transit(TextureTransitInfo& transitInfo) {
    Utils::transitionImageLayout(mContext,mImage,
                                 mMipLevels, mFormat,
                                 transitInfo.src, transitInfo.dst,
                                 transitInfo.level, transitInfo.levelCount,
                                 0, 1);
}

TextureAllocateInfo Texture::getAllocateInfo() const {
    TextureAllocateInfo allocateInfo {
            .format = mFormat,
            .extent = mExtent,
            .numSamples = VK_SAMPLE_COUNT_1_BIT,
            .imageUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevels = mMipLevels,
            .generateMipMaps = mGenerateMipMap
    };
    return allocateInfo;
}

void Texture::generateMipMaps() {
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(mContext->physicalDevice(), mFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        WARNING << "WARNING: Texture image format does not support linear blitting!";
        return;
    }

    auto commandPool = Utils::createCommandPool(mContext, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    VkCommandBuffer commandBuffer = Utils::beginSingleTimeCommands(mContext->device(), commandPool);

    auto mipWidth = static_cast<int32_t>(mExtent.width);
    auto mipHeight = static_cast<int32_t>(mExtent.height);

    for (uint32_t i = 1; i < mMipLevels; ++i) {
        TextureTransitInfoCmd transitInfo{
                .commandBuffer = commandBuffer,
                .src = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .dst = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                .level = i - 1,
                .levelCount = 1,
        };
        transit(transitInfo);

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

        transitInfo.src = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        transitInfo.dst = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        transit(transitInfo);
        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    TextureTransitInfoCmd transitInfo{
            .commandBuffer = commandBuffer,
            .src = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .dst = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .level = mMipLevels - 1,
            .levelCount = 1,
    };
    transit(transitInfo);

    Utils::endSingleTimeCommands(mContext, commandPool, commandBuffer);
    vkDestroyCommandPool(mContext->device(), commandPool, nullptr);
}
