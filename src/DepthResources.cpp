//
// Created by auser on 5/3/25.
//

#include "DepthResources.h"
#include "Utils.h"

DepthResources::DepthResources(Context* context, VkExtent2D swapChainExtent): mContext(context) {
    createDepthResources(swapChainExtent);
}

DepthResources::~DepthResources() {
    clear();
}

void DepthResources::recreate(VkExtent2D swapChainExtent) {
    clear();
    createDepthResources(swapChainExtent);
}

void DepthResources::createDepthResources(VkExtent2D swapChainExtent) {
    VkFormat depthFormat = Utils::findDepthFormat(mContext);
    Utils::createImage(mContext->allocator(), mImageAllocation, VMA_MEMORY_USAGE_AUTO,
                       mImage, 1, swapChainExtent.width, swapChainExtent.height, depthFormat,
                       VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    mImageView = Utils::createImageView(mContext->device(), mImage, 1, VK_IMAGE_VIEW_TYPE_2D,
                                                   depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    Utils::transitionImageLayout(mContext, mImage, 1, depthFormat,
                                 VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void DepthResources::clear() {
    vkDestroyImageView(mContext->device(), mImageView, nullptr);
    vmaDestroyImage(mContext->allocator(), mImage, mImageAllocation);
}

