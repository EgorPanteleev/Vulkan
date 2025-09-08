//
// Created by igor on 9/7/25.
//

#ifndef VULKAN_CUBEMAPIMAGE_HPP
#define VULKAN_CUBEMAPIMAGE_HPP
#include "SampledImage.hpp"

struct CubeMapImageLoadInfo {
    std::vector<std::string> paths;
};

struct CubeMapTransitInfoCmd {
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkImageLayout src = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout dst = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t layer = 0;
    uint32_t layerCount = 1;
};

struct CubeMapTransitInfo {
    VkImageLayout src = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout dst = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t layer = 0;
    uint32_t layerCount = 1;
};

class CubeMapImage: public SampledImage {
public:
    CubeMapImage(Context* context);

    void allocate(ImageAllocateInfo& allocateInfo);
    void load(const CubeMapImageLoadInfo& loadInfo);

    void transit(const CubeMapTransitInfoCmd& transitInfo);
    void transit(const CubeMapTransitInfo& transitInfo);
private:
    bool loadCommon(const std::string& path, uint32_t layer);
    void load(void* data, VkExtent2D extent, uint32_t layer);
};

#endif //VULKAN_CUBEMAPIMAGE_HPP
