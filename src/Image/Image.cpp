//
// Created by igor on 6/30/25.
//

#include "Image.h"
#include "Utils.h"

Image::Image(Context* context): mContext(context) {
}

Image::~Image() {
}

void Image::allocate(ImageAllocateInfo& allocateInfo) {
    mFormat = allocateInfo.format;
    mExtent = allocateInfo.extent;
    Utils::createFullImage(mContext, mImageAllocation, mImage, mImageView, 1, allocateInfo.numSamples,
                           mExtent, mFormat, allocateInfo.imageUsageFlags, allocateInfo.aspectFlags );
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
                                 transitInfo.level, transitInfo.levelCount);
}

void Image::transit(ImageTransitInfo& transitInfo) {
    Utils::transitionImageLayout(mContext,mImage,
                                 1, mFormat,
                                 transitInfo.src, transitInfo.dst,
                                 transitInfo.level, transitInfo.levelCount);
}