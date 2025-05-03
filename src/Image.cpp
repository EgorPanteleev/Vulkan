//
// Created by auser on 4/7/25.
//

#include "Image.h"
#include "Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Image::Image(Context* context, const std::string& path): mContext(context) {
    createImage(path);
    createImageView();
    createSampler();
}

Image::~Image() {
    vkDestroySampler(mContext->device(), mSampler, nullptr);
    vkDestroyImageView(mContext->device(), mImageView, nullptr);
    vmaDestroyImage(mContext->allocator(), mImage, mImageAllocation);
}

//void createImage(VmaAllocator allocator, VmaAllocation& imageAllocation, VmaMemoryUsage allocUsage,
//                 VkImage& image, uint32_t width, uint32_t height, VkFormat format,
//                 VkImageTiling tiling, VkImageUsageFlags imageUsage)
void Image::createImage(const std::string& path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    if (!pixels) {
        throw std::runtime_error("Failed to load texture image!");
    }

    VkBuffer stagingBuffer;
    VmaAllocation allocation;

    Utils::createBuffer(mContext->allocator(), allocation, VMA_MEMORY_USAGE_CPU_ONLY, imageSize,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);

    Utils::copyDataToBuffer( mContext->allocator(), allocation, pixels, static_cast<size_t>(imageSize));

    stbi_image_free(pixels);

    Utils::createImage(mContext->allocator(), mImageAllocation, VMA_MEMORY_USAGE_AUTO,
                       mImage, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
                       VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT );

    Utils::transitionImageLayout(mContext, mImage, VK_FORMAT_R8G8B8A8_SRGB,
                                 VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    Utils::copyBufferToImage(mContext, stagingBuffer, mImage,
                             static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    Utils::transitionImageLayout(mContext, mImage, VK_FORMAT_R8G8B8A8_SRGB,
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vmaDestroyBuffer(mContext->allocator(), stagingBuffer, allocation);
}

void Image::createImageView() {
    mImageView = Utils::createImageView(mContext->device(), mImage, VK_IMAGE_VIEW_TYPE_2D,
                                        VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

}

void Image::createSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 1;
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(mContext->physicalDevice(), &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    if (vkCreateSampler(mContext->device(), &samplerInfo, nullptr, &mSampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler!");
    }

}