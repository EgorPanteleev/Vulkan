//
// Created by igor on 7/5/25.
//

#include "SampledImage.h"
#include "Utils.h"

SampledImage::SampledImage(Context* context): Image(context) {}

SampledImage::~SampledImage() {}

void SampledImage::destroy() {
    vkDestroySampler(mContext->device(), mSampler, nullptr);
    Image::destroy();
}

void SampledImage::allocate(ImageAllocateInfo& allocateInfo) {
    Image::allocate(allocateInfo);
    Utils::createSampler(mContext, mSampler, 1,
                         VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_FALSE);

}