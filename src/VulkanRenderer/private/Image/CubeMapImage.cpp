//
// Created by igor on 9/7/25.
//
#include "CubeMapImage.hpp"
#include <stb_image.h>

CubeMapImage::CubeMapImage(Context* context): SampledImage(context) {
}

void CubeMapImage::allocate(ImageAllocateInfo &allocateInfo) {
    mFormat = allocateInfo.format;
    mExtent = allocateInfo.extent;
    ImageAndViewCreateInfo imageAndViewCreateInfo{
            .context = mContext,
            .imageAllocation = mImageAllocation,
            .image = mImage,
            .flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
            .imageView = mImageView,
            .viewType = VK_IMAGE_VIEW_TYPE_CUBE,
            .mipLevels = 1,
            .numSamples = allocateInfo.numSamples,
            .extent = mExtent,
            .arrayLayers = 6, //cubeMap
            .format = mFormat,
            .imageUsageFlags = allocateInfo.imageUsageFlags,
            .aspectFlags = allocateInfo.aspectFlags
    };
    createImageAndView(imageAndViewCreateInfo);

    SamplerCreateInfo samplerCreateInfo{
            .context = mContext,
            .sampler = mSampler,
            .mipLevels = 1,
            .adressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .compare = VK_FALSE,
    };
    createSampler(samplerCreateInfo);
}

void CubeMapImage::load(const CubeMapImageLoadInfo& loadInfo) {
    int mTexChannels;
    stbi_load(loadInfo.paths[0].c_str(), (int*)&mExtent.width,(int*)&mExtent.height,
                             &mTexChannels, STBI_rgb_alpha);
    ImageAllocateInfo allocateInfo {
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .extent = mExtent,
            .numSamples = VK_SAMPLE_COUNT_1_BIT,
            .imageUsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
    };
    allocate(allocateInfo);
    for (uint32_t i = 0; i < loadInfo.paths.size(); ++i) {
        loadCommon(loadInfo.paths[i], i);
    }
}

bool CubeMapImage::loadCommon(const std::string& path, uint32_t layer) {
    int mTexChannels;
    void* pixels = stbi_load(path.c_str(), (int*)&mExtent.width,(int*)&mExtent.height,
                             &mTexChannels, STBI_rgb_alpha);
    if (!pixels) return false;
    load(pixels, mExtent, layer);
    return true;
}

void CubeMapImage::load(void* data, VkExtent2D extent, uint32_t layer) {
    CubeMapTransitInfo transitInfo{
            .src = VK_IMAGE_LAYOUT_UNDEFINED,
            .dst = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .layer = layer,
            .layerCount = 1,
    };
    transit(transitInfo);
    VkDeviceSize imageSize = extent.width * extent.height * 4;
    VkBuffer stagingBuffer;
    VmaAllocation allocation;

    Utils::createBuffer(mContext->allocator(), allocation, VMA_MEMORY_USAGE_CPU_ONLY, imageSize,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);

    Utils::copyDataToBuffer(mContext->allocator(), allocation, data, (size_t)imageSize);
    Utils::copyBufferToImage(mContext, stagingBuffer, mImage,
                             extent.width, extent.height, 0, layer);

    vmaDestroyBuffer(mContext->allocator(), stagingBuffer, allocation);
}

void CubeMapImage::transit(const CubeMapTransitInfoCmd& transitInfo) {
    Utils::transitionImageLayout(transitInfo.commandBuffer, mImage,
                                 1, mFormat,
                                 transitInfo.src, transitInfo.dst,
                                 0, 1,
                                 transitInfo.layer, transitInfo.layerCount);
}

void CubeMapImage::transit(const CubeMapTransitInfo& transitInfo) {
    Utils::transitionImageLayout(mContext,mImage,
                                 1, mFormat,
                                 transitInfo.src, transitInfo.dst,
                                 0, 1,
                                 transitInfo.layer, transitInfo.layerCount);
}
