//
// Created by igor on 7/5/25.
//

#ifndef VULKAN_SAMPLEDIMAGE_H
#define VULKAN_SAMPLEDIMAGE_H

#include "Image.h"

class SampledImage: public Image {
public:
    SampledImage(Context* context);
    virtual ~SampledImage();

    void allocate(ImageAllocateInfo& allocateInfo);

    VkSampler sampler() { return mSampler; }
    void destroy();
protected:
    VkSampler mSampler;
};


#endif //VULKAN_SAMPLEDIMAGE_H
