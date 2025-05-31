//
// Created by auser on 5/4/25.
//

#include "ColorResources.h"
#include "Utils.h"

ColorResources::ColorResources(Context* context, VkExtent2D swapChainExtent, VkFormat imageFormat): mContext(context) {
    createColorResources(swapChainExtent, imageFormat);
}

ColorResources::~ColorResources() {
    clear();
}

void ColorResources::recreate(VkExtent2D swapChainExtent, VkFormat imageFormat) {
    clear();
    createColorResources(swapChainExtent, imageFormat);
}

void ColorResources::createColorResources(VkExtent2D swapChainExtent, VkFormat imageFormat) {

    auto msaaSamples = Utils::getMaxUsableSampleCount(mContext->physicalDevice());

    Utils::createImage(mContext->allocator(), mImageAllocation, VMA_MEMORY_USAGE_AUTO, mImage, 1, msaaSamples,
                       swapChainExtent.width, swapChainExtent.height, imageFormat, VK_IMAGE_TILING_OPTIMAL,
                       VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

    mImageView = Utils::createImageView(mContext->device(), mImage, 1, VK_IMAGE_VIEW_TYPE_2D,
                                        imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
}

void ColorResources::clear() {
    vkDestroyImageView(mContext->device(), mImageView, nullptr);
    vmaDestroyImage(mContext->allocator(), mImage, mImageAllocation);
}
