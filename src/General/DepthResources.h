//
// Created by auser on 5/3/25.
//

#ifndef VULKAN_DEPTHRESOURCES_H
#define VULKAN_DEPTHRESOURCES_H

#include "Context.h"

class DepthResources {
public:
    DepthResources(Context* context, VkExtent2D swapChainExtent);
    ~DepthResources();

    VkImage image() { return mImage; }
    VkImageView imageView() { return mImageView; }
    VkSampler sampler() { return mSampler; }

    VkImage shadowImage() { return mShadowImage; }
    VkImageView shadowImageView() { return mShadowImageView; }
    VkExtent2D shadowMapExtent() { return mShadowMapExtent; }

    void translateShadowImage(VkCommandBuffer commandBuffer, VkImageLayout from, VkImageLayout to);

    void recreate(VkExtent2D swapChainExtent);
private:
    void createDepthResources(VkExtent2D swapChainExtent);

    void clear();

    Context* mContext;
    VkImage mImage;
    VmaAllocation mImageAllocation;
    VkImageView mImageView;

    VkImage mShadowImage;
    VmaAllocation mShadowImageAllocation;
    VkImageView mShadowImageView;
    VkSampler mSampler;
    VkExtent2D mShadowMapExtent;
};


#endif //VULKAN_DEPTHRESOURCES_H
