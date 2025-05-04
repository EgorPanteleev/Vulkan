//
// Created by auser on 5/4/25.
//

#ifndef VULKAN_COLORRESOURCES_H
#define VULKAN_COLORRESOURCES_H

#include "Context.h"
#include "SwapChain.h"

class ColorResources {
public:
    ColorResources(Context* context, VkExtent2D swapChainExtent, VkFormat imageFormat);
    ~ColorResources();

    void recreate(VkExtent2D swapChainExtent, VkFormat imageFormat);

    VkImageView imageView() { return mImageView; }
private:
    void createColorResources(VkExtent2D swapChainExtent, VkFormat imageFormat);
    void clear();
    Context* mContext;
    VkImage mImage;
    VmaAllocation mImageAllocation;
    VkImageView mImageView;
};


#endif //VULKAN_COLORRESOURCES_H
