//
// Created by auser on 4/4/25.
//

#include "CommandManager.h"
#include "Utils.h"
#include "DescriptorSet.h"

CommandManager::CommandManager(Context* context): mContext(context) {
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
                                         DescriptorSet* descriptorSet, ShadowDescriptorSet* shadowDescriptorSet,
                                         VertexBuffer* vertexBuffer, uint32_t imageIndex) {
    uint32_t currentFrame = swapChain->currentFrame();
    auto commandBuffer = mCommandBuffers[ currentFrame ];
    vkResetCommandBuffer( commandBuffer, 0 );
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // SHADOW PASS BEGIN
    VkRenderPassBeginInfo shadowPassInfo{};
    shadowPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    shadowPassInfo.renderPass = shadowPipeline->renderPass();
    shadowPassInfo.framebuffer = swapChain->shadowFrameBuffers()[0];
    shadowPassInfo.renderArea.offset = {0, 0};
    shadowPassInfo.renderArea.extent = {1024,1024};

    VkClearValue shadowClearValue{};
    shadowClearValue.depthStencil = {1.0f, 0};
    shadowPassInfo.clearValueCount = 1;
    shadowPassInfo.pClearValues = &shadowClearValue;

    vkCmdBeginRenderPass(commandBuffer, &shadowPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipeline->graphicsPipeline());

    // Set viewport and scissor for shadow pipeline
    VkViewport shadowViewport{};
    shadowViewport.x = 0.0f;
    shadowViewport.y = 0.0f;
    shadowViewport.width = static_cast<float>(1024);
    shadowViewport.height = static_cast<float>(1024);
    shadowViewport.minDepth = 0.0f;
    shadowViewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &shadowViewport);

    VkRect2D shadowScissor{};
    shadowScissor.offset = {0, 0};
    shadowScissor.extent = {1024,1024};
    vkCmdSetScissor(commandBuffer, 0, 1, &shadowScissor);

    VkBuffer vertexBuffers[] = {vertexBuffer->vertexBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, vertexBuffer->indexBuffer(), 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipeline->pipelineLayout(),
                            0, 1, &shadowDescriptorSet->descriptorSets()[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(vertexBuffer->indices().size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    // SHADOW PASS END

    // Transition Shadow Map to Read-Only Optimal for Sampling
//    Utils::transitionImageLayout(mContext, shadowPipeline->(), 1, shadowPipeline->depthFormat(),
//                                 VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = graphicsPipeline->renderPass();
    renderPassInfo.framebuffer = swapChain->frameBuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain->extent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline() );

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain->extent().width);
    viewport.height = static_cast<float>(swapChain->extent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain->extent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline());

//    VkBuffer vertexBuffers[] = {vertexBuffer->vertexBuffer()};
//    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, vertexBuffer->indexBuffer(), 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipelineLayout(),
                            0, 1, &descriptorSet->descriptorSets()[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(vertexBuffer->indices().size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

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