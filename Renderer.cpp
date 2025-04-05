//
// Created by auser on 4/4/25.
//

#include "Renderer.h"

Renderer::Renderer(): mContext(), mSwapChain(&mContext),
                      mGraphicsPipeline(&mContext, &mSwapChain,
                                        "/home/auser/dev/src/Vulkan/CompiledShaders/shader.vert.spv",
                                        "/home/auser/dev/src/Vulkan/CompiledShaders/shader.frag.spv"),
                      mCommandManager(&mContext), mSyncObjects(&mContext) {
    mSwapChain.createFrameBuffers(mGraphicsPipeline.renderPass());
}

Renderer::~Renderer() {

}

void Renderer::run() {
    mainLoop();
}

void Renderer::mainLoop() {
    while ( !mContext.window().shouldClose() ) {
        glfwPollEvents();
        drawFrame();
    }
    vkDeviceWaitIdle( mContext.device() );
}

void Renderer::drawFrame() {
    uint32_t imageIndex;
    auto acquireResult = mSwapChain.acquireNextImage(&imageIndex,
                                 mSyncObjects.imageAvailableSemaphore(mSwapChain.currentFrame()),
                                 mSyncObjects.inFlightFence(mSwapChain.currentFrame()));

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR ||
        acquireResult == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        recreateSwapChain();
        return;
    } else if (acquireResult != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    // Only reset the fence if we are submitting work
    vkResetFences(mContext.device(), 1, &mSyncObjects.inFlightFence(mSwapChain.currentFrame()));

    mCommandManager.recordCommandBuffer( &mSwapChain, &mGraphicsPipeline, imageIndex );
    auto submitResult = mCommandManager.submitCommandBuffer( &mSwapChain, &mSyncObjects, &imageIndex );
    if (submitResult == VK_ERROR_OUT_OF_DATE_KHR ||
            submitResult == VK_SUBOPTIMAL_KHR || framebufferResized) {
        recreateSwapChain();
        return;
    } else if (submitResult != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit command buffer!");
    }
    mSwapChain.updateCurrentFrame();
}

void Renderer::recreateSwapChain() {
    vkDeviceWaitIdle(mContext.device());
    mSwapChain.clear();
    mSwapChain.recreate();
    mSwapChain.createFrameBuffers(mGraphicsPipeline.renderPass());
}