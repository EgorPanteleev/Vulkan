//
// Created by auser on 3/23/25.
//

#ifndef VULKAN_RENDERPASS_H
#define VULKAN_RENDERPASS_H

#include "Device.h"

class RenderPass {
public:
    RenderPass( Device& device, VkFormat swapChainImageFormat );
    ~RenderPass();

    VkRenderPass& operator()() { return renderPass; }

private:
    void createRenderPass( VkFormat swapChainImageFormat );

    Device& device;
    VkRenderPass renderPass;
};


#endif //VULKAN_RENDERPASS_H
