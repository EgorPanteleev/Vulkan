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
    auto msaaSamples = Utils::getMaxUsableSampleCount(mContext->physicalDevice());
    Utils::createImage(mContext->allocator(), mImageAllocation, VMA_MEMORY_USAGE_AUTO,
                       mImage, 1, msaaSamples, swapChainExtent.width, swapChainExtent.height, depthFormat,
                       VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    mImageView = Utils::createImageView(mContext->device(), mImage, 1, VK_IMAGE_VIEW_TYPE_2D,
                                                   depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    Utils::transitionImageLayout(mContext, mImage, 1, depthFormat,
                                 VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);



    Utils::createImage(mContext->allocator(), mShadowImageAllocation, VMA_MEMORY_USAGE_AUTO,
                       mShadowImage, 1, VK_SAMPLE_COUNT_1_BIT, 1024, 1024, depthFormat,
                       VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    mShadowImageView = Utils::createImageView(mContext->device(), mShadowImage, 1, VK_IMAGE_VIEW_TYPE_2D,
                                        depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    Utils::transitionImageLayout(mContext, mShadowImage, 1, depthFormat,
                                 VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    Utils::createSampler(mContext, mSampler, 1);
}

void DepthResources::clear() {
    vkDestroySampler(mContext->device(), mSampler, nullptr);
    vkDestroyImageView(mContext->device(), mShadowImageView, nullptr);
    vmaDestroyImage(mContext->allocator(), mShadowImage, mShadowImageAllocation);

    vkDestroyImageView(mContext->device(), mImageView, nullptr);
    vmaDestroyImage(mContext->allocator(), mImage, mImageAllocation);
}

