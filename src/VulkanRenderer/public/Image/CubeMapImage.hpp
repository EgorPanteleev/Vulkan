//
// Created by igor on 9/7/25.
//

#ifndef VULKAN_CUBEMAPIMAGE_HPP
#define VULKAN_CUBEMAPIMAGE_HPP
#include "SampledImage.hpp"

struct CubeMapImageLoadInfo {

};

class CubeMapImage: public SampledImage {
    void allocate(ImageAllocateInfo& allocateInfo);
    void load(const CubeMapImageLoadInfo& loadInfo);
};

#endif //VULKAN_CUBEMAPIMAGE_HPP
