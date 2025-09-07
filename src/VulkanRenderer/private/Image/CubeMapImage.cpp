//
// Created by igor on 9/7/25.
//
#include "CubeMapImage.hpp"

void CubeMapImage::allocate(ImageAllocateInfo &allocateInfo) {
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
            .arrayLayers = 6, //cubeMap
            .format = mFormat,
            .imageUsageFlags = allocateInfo.imageUsageFlags,
            .aspectFlags = allocateInfo.aspectFlags
    };
    createImageAndView(imageAndViewCreateInfo);
}

void CubeMapImage::load(const CubeMapImageLoadInfo& loadInfo) {

}