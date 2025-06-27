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

struct CommandManagerRecordInfo{
    SwapChain* swapChain;
    GraphicsPipeline* graphicsPipeline;
    ShadowPipeline* shadowPipeline;
    VkImGui* vkImGui;
    VertexBuffer* vertexBuffer;
    uint32_t imageIndex;
    uint32_t currentFrame;
};

struct CommandManagerSubmitInfo{
    SwapChain* swapChain;
    SyncObjects* syncObjects;
    uint32_t imageIndex;
    uint32_t currentFrame;
};

class CommandManager {
public:
    CommandManager(Context* context, DepthResources* depthResources);
    ~CommandManager();
    void recordCommandBuffer(CommandManagerRecordInfo& recordInfo);
    VkResult submitCommandBuffer(CommandManagerSubmitInfo& submitInfo);

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
