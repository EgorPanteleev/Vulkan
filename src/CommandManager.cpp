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

void CommandManager::recordCommandBuffer(CommandManagerRecordInfo& recordInfo) {
    uint32_t currentFrame = recordInfo.currentFrame;
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
        .frameBuffer = recordInfo.swapChain->shadowFrameBuffers()[0],
        .vertexBuffer = recordInfo.vertexBuffer->vertexBuffer(),
        .indexBuffer = recordInfo.vertexBuffer->indexBuffer(),
        .indexCount = (uint32_t)recordInfo.vertexBuffer->indices().size(),
        .currentFrame = currentFrame
    };
    recordInfo.shadowPipeline->render(shadowPipelineRenderInfo);

    GraphicsPipelineRenderInfo graphicsPipelineRenderInfo{
            .commandBuffer = commandBuffer,
            .frameBuffer = recordInfo.swapChain->frameBuffers()[recordInfo.imageIndex],
            .vertexBuffer = recordInfo.vertexBuffer->vertexBuffer(),
            .indexBuffer = recordInfo.vertexBuffer->indexBuffer(),
            .indexCount = (uint32_t)recordInfo.vertexBuffer->indices().size(),
            .currentFrame = currentFrame,
            .extent = recordInfo.swapChain->extent()
    };
    recordInfo.graphicsPipeline->render(graphicsPipelineRenderInfo);

    if ( recordInfo.vkImGui ) recordInfo.vkImGui->render(commandBuffer, recordInfo.imageIndex);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

VkResult CommandManager::submitCommandBuffer(CommandManagerSubmitInfo& submitInfo) {
    uint32_t currentFrame = submitInfo.currentFrame;
    VkSubmitInfo vkSubmitInfo{};
    vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { submitInfo.syncObjects->imageAvailableSemaphore( currentFrame ) };
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    vkSubmitInfo.waitSemaphoreCount = 1;
    vkSubmitInfo.pWaitSemaphores = waitSemaphores;
    vkSubmitInfo.pWaitDstStageMask = waitStages;
    vkSubmitInfo.commandBufferCount = 1;
    vkSubmitInfo.pCommandBuffers = &mCommandBuffers[currentFrame];
    VkSemaphore signalSemaphores[] = { submitInfo.syncObjects->renderFinishedSemaphore( submitInfo.imageIndex ) };
    vkSubmitInfo.signalSemaphoreCount = 1;
    vkSubmitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit( mContext->graphicsQueue(), 1, &vkSubmitInfo, submitInfo.syncObjects->inFlightFence( currentFrame ) ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { submitInfo.swapChain->swapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &submitInfo.imageIndex;
    presentInfo.pResults = nullptr;
    auto result = vkQueuePresentKHR( mContext->presentQueue(), &presentInfo );
    return result;
}