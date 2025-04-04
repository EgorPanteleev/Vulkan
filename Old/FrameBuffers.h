//
// Created by auser on 3/23/25.
//

#ifndef VULKAN_FRAMEBUFFERS_H
#define VULKAN_FRAMEBUFFERS_H

#include "Device.h"
#include "SwapChain_old.h"
#include "RenderPass.h"

class FrameBuffers {
public:
    FrameBuffers(Device& device, SwapChain_old& swapChain, RenderPass& renderPass );

    ~FrameBuffers();

    std::vector<VkFramebuffer>& getFrameBuffers() { return frameBuffers; }
private:
    void createFrameBuffers();

    Device& device;
    SwapChain_old& swapChain;
    RenderPass& renderPass;
    std::vector<VkFramebuffer> frameBuffers;
};


#endif //VULKAN_FRAMEBUFFERS_H
