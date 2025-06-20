//
// Created by auser on 4/4/25.
//

#include "CommandManager.h"
#include "Utils.h"
#include "DescriptorSet.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include "VkImGui.h"
CommandManager::CommandManager(Context* context, DepthResources* depthResources):
                               mContext(context), mDepthResources(depthResources) {
    createCommandPool();
    createCommandBuffers();
}

CommandManager::~CommandManager() {
    vkFreeCommandBuffers( mContext->device(), mCommandPool,
                          mCommandBuffers.size(), mCommandBuffers.data() );
    vkDestroyCommandPool(mContext->device(), mCommandPool, nullptr);
}

void CommandManager::createCommandPool() {
    mCommandPool = Utils::createCommandPool(mContext, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );
}

void CommandManager::createCommandBuffers() {
    mCommandBuffers.resize( mContext->maxFramesInFlight() );

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) mCommandBuffers.size();

    if (vkAllocateCommandBuffers(mContext->device(), &allocInfo, mCommandBuffers.data() ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
    INFO << "Created command buffers";
}

void CommandManager::recordCommandBuffer(SwapChain* swapChain, GraphicsPipeline* graphicsPipeline, ShadowPipeline* shadowPipeline,
                                         VertexBuffer* vertexBuffer, uint32_t imageIndex, VkImGui* vkImGui) {
    uint32_t currentFrame = swapChain->currentFrame();
    auto commandBuffer = mCommandBuffers[currentFrame];
    vkResetCommandBuffer(commandBuffer, 0);
    VkCommandBufferBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0,
            .pInheritanceInfo = nullptr
    };
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    ShadowPipelineRenderInfo shadowPipelineRenderInfo {
        .commandBuffer = commandBuffer,
        .frameBuffer = swapChain->shadowFrameBuffers()[0],
        .vertexBuffer = vertexBuffer->vertexBuffer(),
        .indexBuffer = vertexBuffer->indexBuffer(),
        .indexCount = (uint32_t)vertexBuffer->indices().size(),
        .currentFrame = currentFrame
    };
    shadowPipeline->render(shadowPipelineRenderInfo);

    GraphicsPipelineRenderInfo graphicsPipelineRenderInfo{
            .commandBuffer = commandBuffer,
            .frameBuffer = swapChain->frameBuffers()[imageIndex],
            .vertexBuffer = vertexBuffer->vertexBuffer(),
            .indexBuffer = vertexBuffer->indexBuffer(),
            .indexCount = (uint32_t)vertexBuffer->indices().size(),
            .currentFrame = currentFrame,
            .extent = swapChain->extent()
    };
    graphicsPipeline->render(graphicsPipelineRenderInfo);

    if ( vkImGui ) vkImGui->render(commandBuffer, imageIndex);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

VkResult CommandManager::submitCommandBuffer(SwapChain* swapChain, SyncObjects* syncObjects, uint32_t* imageIndex) {
    uint32_t currentFrame = swapChain->currentFrame();
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { syncObjects->imageAvailableSemaphore( currentFrame ) };
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffers[currentFrame];
    VkSemaphore signalSemaphores[] = { syncObjects->renderFinishedSemaphore( *imageIndex ) };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit( mContext->graphicsQueue(), 1, &submitInfo, syncObjects->inFlightFence( currentFrame ) ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { swapChain->swapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;
    presentInfo.pResults = nullptr; // Optional
    auto result = vkQueuePresentKHR( mContext->presentQueue(), &presentInfo );
    return result;
}