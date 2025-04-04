//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_COMMANDMANAGER_H
#define VULKAN_COMMANDMANAGER_H

#include "Context.h"

class CommandManager {
public:
    const uint32_t maxFramesInFlight = 2; //TODO
    CommandManager(Context* context);
    ~CommandManager();
private:
    /**
    * Creating command pool
    */
    void createCommandPool();
    /**
    * Creating command buffers
    */
    void createCommandBuffers();
    //void recordCommandBuffer(SwapChain* swapChain, GraphicsPipeline* graphicsPipeline, uint32_t imageIndex);


    Context* mContext;
    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;
};


#endif //VULKAN_COMMANDMANAGER_H
