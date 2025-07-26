//
// Created by auser on 6/12/25.
//

#include "Texture.hpp"
#include "Utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(Context* context): SampledImage(context) {
}

Texture::~Texture() {
}

void Texture::load(TextureLoadInfo& loadInfo) {
    if (loadInfo.data) loadByData(loadInfo);
    else if (!loadInfo.path.empty()) loadByPath(loadInfo);
    else throw std::runtime_error("Can't load texture with given load info!");
}

void Texture::loadByData(TextureLoadInfo& loadInfo) {
    TextureAllocateInfo allocateInfo = getAllocateInfo(loadInfo);
    allocate(allocateInfo);
    mGenerateMipMap = loadInfo.generateMipMap;
    load(loadInfo.data, mExtent, 0);
}

void Texture::loadByPath(TextureLoadInfo& loadInfo) {
    mFormat = toVkFormat(loadInfo.texType);
    mGenerateMipMap = loadInfo.generateMipMap;
    bool status = loadCommon(loadInfo.path);
    if (!status) status = loadCompressed(loadInfo.path);
    if (!status) throw std::runtime_error("Failed to load texture!");
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


bool Texture::loadCommon(const std::string& path) {
    void* pixels = stbi_load(path.c_str(), (int*)&mExtent.width,(int*)&mExtent.height,
                             (int*)&mTexChannels, STBI_rgb_alpha);
    if (!pixels) return false;
    if (mGenerateMipMap) mMipLevels = calcMipLevels(mExtent.width, mExtent.height);
    TextureAllocateInfo allocateInfo = getAllocateInfo();
    allocate(allocateInfo);
    load(pixels, mExtent, 0);
    return true;
}

bool Texture::loadCompressed(const std::string& path) {
    gli::texture tex = gli::load(path.c_str());
    if (tex.empty()) return false;
    TextureAllocateInfo allocateInfo = getAllocateInfo(tex);
    allocate(allocateInfo);
    int layer = 0; int face = 0;
    for (int level = 0; level < tex.levels(); ++level) {
        void* pixels = tex.data(layer, face, level);
        VkExtent2D extent = { (uint32_t)tex.extent(level).x, (uint32_t)tex.extent(level).y };
        load(pixels, extent, level);
    }
    return true;
}


void Texture::load(void* data, VkExtent2D extent, int mipLevel) {
    ImageTransitInfo transitInfo{
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
    Utils::createFullImage(mContext, mImageAllocation, mImage, mImageView, mMipLevels, allocateInfo.numSamples,
                           mExtent, mFormat, allocateInfo.imageUsageFlags, allocateInfo.aspectFlags );
    Utils::createSampler(mContext, mSampler, mMipLevels,
                         VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_FALSE);
}


VkFormat Texture::toVkFormat(ModelTexture::Type modelTexType) {
    VkFormat res;
    switch(modelTexType) {
        case ModelTexture::Type::DIFFUSE:
        case ModelTexture::Type::SPECULAR:
        case ModelTexture::Type::SHININESS:
        case ModelTexture::Type::AMBIENT:
            res = VK_FORMAT_R8G8B8A8_SRGB;
            break;
        case ModelTexture::Type::NORMAL:
            res = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        default:
            INFO << "ID: " << modelTexType;
            throw std::runtime_error("Unsupported model texture format!");
    }
    return res;
}

VkFormat Texture::toVkFormat(gli::texture::format_type gliFormat) {
    VkFormat res;
    switch(gliFormat) {
        case gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8:
            res = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
            break;
        case gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16:
            res = VK_FORMAT_BC3_UNORM_BLOCK;
            break;
        case gli::FORMAT_RG_ATI2N_UNORM_BLOCK16:
            res = VK_FORMAT_BC5_UNORM_BLOCK;
            break;
        default:
            INFO << "ID: " << gliFormat;
            throw std::runtime_error("Unsupported gli format!");
    }
    return res;
}

void Texture::transit(ImageTransitInfoCmd& transitInfo) {
    Utils::transitionImageLayout(transitInfo.commandBuffer, mImage,
                                 mMipLevels, mFormat,
                                 transitInfo.src, transitInfo.dst,
                                 transitInfo.level, transitInfo.levelCount);
}

void Texture::transit(ImageTransitInfo& transitInfo) {
    Utils::transitionImageLayout(mContext,mImage,
                                 mMipLevels, mFormat,
                                 transitInfo.src, transitInfo.dst,
                                 transitInfo.level, transitInfo.levelCount);
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

TextureAllocateInfo Texture::getAllocateInfo(TextureLoadInfo& loadInfo) const {
    TextureAllocateInfo allocateInfo {
            .format = toVkFormat(loadInfo.texType),
            .extent = {loadInfo.width, loadInfo.height},
            .numSamples = VK_SAMPLE_COUNT_1_BIT,
            .imageUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevels = calcMipLevels(loadInfo.width, loadInfo.height),
            .generateMipMaps = loadInfo.generateMipMap
    };
    return allocateInfo;
}

TextureAllocateInfo Texture::getAllocateInfo(const gli::texture& tex) const {
    TextureAllocateInfo allocateInfo {
            .format = toVkFormat(tex.format()),
            .extent = {static_cast<uint32_t>(tex.extent().x), static_cast<uint32_t>(tex.extent().y)},
            .numSamples = VK_SAMPLE_COUNT_1_BIT,
            .imageUsageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevels = static_cast<uint32_t>(tex.levels()),
            .generateMipMaps = false // Compressed images doesnt support generating mipMaps
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
        ImageTransitInfoCmd transitInfo{
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

    ImageTransitInfoCmd transitInfo{
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
