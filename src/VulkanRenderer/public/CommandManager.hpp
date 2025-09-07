//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_COMMANDMANAGER_H
#define VULKAN_COMMANDMANAGER_H

#include "Context.hpp"
#include "SwapChain.hpp"
#include "GraphicsPipeline.hpp"
#include "SyncObjects.hpp"
#include "VertexBuffer.hpp"
#include "ShadowDescriptorSet.hpp"
#include "ShadowPipeline.hpp"
#include "CubeMapPipeline.hpp"

#include <imgui.h>

class VkImGui;

struct CommandManagerRecordInfo{
    SwapChain* swapChain;
    GraphicsPipeline* graphicsPipeline;
    ShadowPipeline* shadowPipeline;
    CubeMapPipeline* cubeMapPipeline;
    VkImGui* vkImGui;
    VertexBuffer<Vertex>* vertexBuffer;
    VertexBuffer<glm::vec3>* skyBoxVertexBuffer;
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
    CommandManager(Context* context);
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
    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;
};


#endif //VULKAN_COMMANDMANAGER_H
