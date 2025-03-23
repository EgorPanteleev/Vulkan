//
// Created by auser on 3/23/25.
//

#ifndef VULKAN_COMMANDBUFFERS_H
#define VULKAN_COMMANDBUFFERS_H

#include "Device.h"
#include "RenderPass.h"
#include "FrameBuffers.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "SyncObjects.h"

class CommandBuffers {
public:
    CommandBuffers(Device& device, SwapChain& swapChain, RenderPass& renderPass,
                   FrameBuffers& frameBuffers, GraphicsPipeline& graphicsPipeline,
                   SyncObjects& syncObjects );

    ~CommandBuffers();

    VkCommandBuffer operator()( uint32_t imageIndex ) { return commandBuffers[imageIndex]; }

    void recordCommandBuffer( uint32_t imageIndex );

    VkResult submitCommandBuffer( uint32_t *imageIndex );
private:
    void createCommandBuffers();

    Device& device;
    SwapChain& swapChain;
    RenderPass& renderPass;
    FrameBuffers& frameBuffers;
    GraphicsPipeline& graphicsPipeline;
    SyncObjects& syncObjects;

    std::vector<VkCommandBuffer> commandBuffers;
};


#endif //VULKAN_COMMANDBUFFERS_H
