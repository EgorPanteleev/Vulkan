//
// Created by igor on 6/30/25.
//

#include "Image.hpp"
#include "Utils.hpp"

Image::Image(Context* context): mContext(context) {
}

Image::~Image() {
}

void Image::allocate(ImageAllocateInfo& allocateInfo) {
    mFormat = allocateInfo.format;
    mExtent = allocateInfo.extent;
    ImageAndViewCreateInfo imageAndViewCreateInfo{
        .context = mContext,
        .imageAllocation = mImageAllocation,
        .image = mImage,
        .imageView = mImageView,
        .mipLevels = 1,
        .numSamples = allocateInfo.numSamples,
        .extent = mExtent,
        .arrayLayers = 1,
        .format = mFormat,
        .imageUsageFlags = allocateInfo.imageUsageFlags,
        .aspectFlags = allocateInfo.aspectFlags
    };
    createImageAndView(imageAndViewCreateInfo);
}

void Image::destroy() {
    vkDestroyImageView(mContext->device(), mImageView, nullptr);
    vmaDestroyImage(mContext->allocator(), mImage, mImageAllocation);
}

uint32_t Image::calcMipLevels(uint32_t width, uint32_t height) {
    return std::floor(std::log2(std::max(width, height))) + 1;
}


void Image::transit(ImageTransitInfoCmd& transitInfo) {
    Utils::transitionImageLayout(transitInfo.commandBuffer, mImage,
                                 1, mFormat,
                                 transitInfo.src, transitInfo.dst,
                                 0, 1,
                                 0, 1);
}

void Image::transit(ImageTransitInfo& transitInfo) {
    Utils::transitionImageLayout(mContext,mImage,
                                 1, mFormat,
                                 transitInfo.src, transitInfo.dst,
                                 0, 1,
                                 0, 1);
}

void Image::createImage(const ImageCreateInfo& createInfo) {
    VkImageCreateInfo imageInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = createInfo.format,
            .extent = {
                    .width = createInfo.width,
                    .height = createInfo.height,
                    .depth = 1,
            },
            .mipLevels = createInfo.mipLevels,
            .arrayLayers = createInfo.arrayLayers,
            .samples = createInfo.numSamples,
            .tiling = createInfo.tiling,
            .usage = createInfo.imageUsage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VmaAllocationCreateInfo allocInfo{
            .usage = createInfo.allocUsage,
            .preferredFlags = 0,
    };

    VkResult result = vmaCreateImage(
            createInfo.allocator, &imageInfo, &allocInfo,
            &createInfo.image, &createInfo.imageAllocation, nullptr
    );
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image!");
    }
}

VkImageView Image::createImageView(const ImageViewCreateInfo& createInfo) {
    VkImageView imageView;
    VkImageViewCreateInfo imageViewInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = createInfo.image,
            .viewType = createInfo.viewType,
            .format = createInfo.format,
            .components = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                    .aspectMask = createInfo.aspectFlags,
                    .baseMipLevel = 0,
                    .levelCount = createInfo.mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount = 1
            },
    };
    VK_CHECK(vkCreateImageView(createInfo.device, &imageViewInfo, nullptr, &imageView),
             "Failed to create image view!");
    return imageView;
}

void Image::createImageAndView(const ImageAndViewCreateInfo& createInfo) {
    ImageCreateInfo imageCreateInfo {
            .allocator = createInfo.context->allocator(),
            .imageAllocation = createInfo.imageAllocation,
            .allocUsage = VMA_MEMORY_USAGE_AUTO,
            .image = createInfo.image,
            .mipLevels = createInfo.mipLevels,
            .numSamples = createInfo.numSamples,
            .width = createInfo.extent.width,
            .height = createInfo.extent.height,
            .arrayLayers = createInfo.arrayLayers,
            .format = createInfo.format,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .imageUsage = createInfo.imageUsageFlags
    };
    createImage(imageCreateInfo);

    ImageViewCreateInfo imageViewCreateInfo{
        .device = createInfo.context->device(),
        .image = createInfo.image,
        .mipLevels = createInfo.mipLevels,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = createInfo.format,
        .aspectFlags = createInfo.aspectFlags,
    };
    createInfo.imageView = createImageView(imageViewCreateInfo);
}

void Image::createSampler(const SamplerCreateInfo& createInfo) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(createInfo.context->physicalDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = createInfo.adressMode,
            .addressModeV = createInfo.adressMode,
            .addressModeW = createInfo.adressMode,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
            .compareEnable = createInfo.compare,
            .compareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .minLod = 0,
            .maxLod = static_cast<float>(createInfo.mipLevels),
            .borderColor = createInfo.borderColor,
            .unnormalizedCoordinates = VK_FALSE,
    };
    if (vkCreateSampler(createInfo.context->device(), &samplerInfo, nullptr, &createInfo.sampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler!");
    }
}
