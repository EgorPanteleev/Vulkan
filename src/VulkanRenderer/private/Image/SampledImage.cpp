//
// Created by igor on 7/5/25.
//

#include "SampledImage.hpp"
#include "Utils.hpp"

SampledImage::SampledImage(Context* context): Image(context) {}

SampledImage::~SampledImage() {}

void SampledImage::destroy() {
    vkDestroySampler(mContext->device(), mSampler, nullptr);
    Image::destroy();
}

void SampledImage::allocate(ImageAllocateInfo& allocateInfo) {
    Image::allocate(allocateInfo);
    SamplerCreateInfo samplerCreateInfo{
        .context = mContext,
        .sampler = mSampler,
        .mipLevels = 1,
        .adressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        .compare = VK_FALSE,
    };
    createSampler(samplerCreateInfo);
}