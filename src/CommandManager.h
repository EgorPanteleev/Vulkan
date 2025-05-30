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

class CommandManager {
public:
    CommandManager(Context* context);
    ~CommandManager();
    void recordCommandBuffer(SwapChain* swapChain, GraphicsPipeline* graphicsPipeline, ShadowPipeline* shadowPipeline,
                             DescriptorSet* descriptorSet, ShadowDescriptorSet* shadowDescriptorSet,
                             VertexBuffer* vertexBuffer, uint32_t imageIndex);
    VkResult submitCommandBuffer(SwapChain* swapChain, SyncObjects* syncObjects, uint32_t* imageIndex);
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
    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;
};


#endif //VULKAN_COMMANDMANAGER_H
