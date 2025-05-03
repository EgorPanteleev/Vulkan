//
// Created by auser on 4/7/25.
//

#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include "Context.h"

class Image {
public:
    Image(Context* context);
    ~Image();

    VkImage image() { return mImage; }
    VmaAllocation imageAllocation() { return mImageAllocation; }
    VkImageView imageView() { return mImageView; }
    VkSampler sampler() { return mSampler; }
private:
    void createImage();
    void createImageView();
    void createSampler(); //Is it must be here?

    Context* mContext;
    VkImage mImage;
    VmaAllocation mImageAllocation;
    VkImageView mImageView;
    VkSampler mSampler;
};


#endif //VULKAN_IMAGE_H
