//
// Created by auser on 3/23/25.
//

#ifndef VULKAN_COMMANDBUFFER_H
#define VULKAN_COMMANDBUFFER_H

#include "Device.h"
#include "RenderPass.h"
#include "FrameBuffers.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"

class CommandBuffer {
public:
    CommandBuffer( Device& device, SwapChain& swapChain, RenderPass& renderPass,
                   FrameBuffers& frameBuffers, GraphicsPipeline& graphicsPipeline );

    ~CommandBuffer();

private:
    void createCommandBuffer();

    void recordCommandBuffer( VkCommandBuffer buffer, uint32_t imageInde );

    Device& device;
    SwapChain& swapChain;
    RenderPass& renderPass;
    FrameBuffers& frameBuffers;
    GraphicsPipeline& graphicsPipeline;

    VkCommandBuffer commandBuffer;
};


#endif //VULKAN_COMMANDBUFFER_H
