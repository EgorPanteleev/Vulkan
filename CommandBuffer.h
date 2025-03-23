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
#include "SyncObjects.h"

class CommandBuffer {
public:
    CommandBuffer( Device& device, SwapChain& swapChain, RenderPass& renderPass,
                   FrameBuffers& frameBuffers, GraphicsPipeline& graphicsPipeline, SyncObjects& syncObjects );

    ~CommandBuffer();

    VkCommandBuffer operator()() { return commandBuffer; }

    void recordCommandBuffer( VkCommandBuffer buffer, uint32_t imageIndex );

    VkResult submitCommandBuffer( VkCommandBuffer buffer, uint32_t *imageIndex );
private:
    void createCommandBuffer();

    Device& device;
    SwapChain& swapChain;
    RenderPass& renderPass;
    FrameBuffers& frameBuffers;
    GraphicsPipeline& graphicsPipeline;
    SyncObjects& syncObjects;

    VkCommandBuffer commandBuffer;
};


#endif //VULKAN_COMMANDBUFFER_H
