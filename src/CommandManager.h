//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_COMMANDMANAGER_H
#define VULKAN_COMMANDMANAGER_H

#include "Context.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "SyncObjects.h"
#include "VertexBuffer.h"
#include "ShadowDescriptorSet.h"
#include "ShadowPipeline.h"

#include <imgui.h>

class VkImGui;

class CommandManager {
public:
    CommandManager(Context* context, DepthResources* depthResources);
    ~CommandManager();
    void recordCommandBuffer(SwapChain* swapChain, GraphicsPipeline* graphicsPipeline, ShadowPipeline* shadowPipeline,
                             VertexBuffer* vertexBuffer, uint32_t imageIndex, VkImGui* vkImGui);
    VkResult submitCommandBuffer(SwapChain* swapChain, SyncObjects* syncObjects, uint32_t* imageIndex);

    VkCommandBuffer commandBuffer(uint32_t currentFrame) const { return mCommandBuffers[currentFrame]; }
private:
    /**
    * Creating command pool
    */
    void createCommandPool();
    /**
    * Creating command buffers
    */
    void createCommandBuffers();


    Context* mContext;
    DepthResources* mDepthResources;
    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;
};


#endif //VULKAN_COMMANDMANAGER_H
