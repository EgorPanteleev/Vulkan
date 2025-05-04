//
// Created by auser on 4/4/25.
//

#include "Renderer.h"
#include "Clock.h"
#include "MessageLogger.h"

Renderer::Renderer(): mContext(), image(&mContext, "/home/auser/dev/src/Vulkan/models/viking_room/viking_room.png"),
                      mSwapChain(&mContext), mDepthResources(&mContext, mSwapChain.extent()), mUniformBuffers(&mContext),
                      mDescriptorSet(&mContext, &image, &mUniformBuffers),
                      mGraphicsPipeline(&mContext, &mSwapChain, &mDescriptorSet,
                                        "/home/auser/dev/src/Vulkan/compiled_shaders/shader.vert.spv",
                                        "/home/auser/dev/src/Vulkan/compiled_shaders/shader.frag.spv"),
                      mCommandManager(&mContext), mVertexBuffer(&mContext), mSyncObjects(&mContext) {
    mSwapChain.createFrameBuffers(mGraphicsPipeline.renderPass(), mDepthResources.imageView());
}

Renderer::~Renderer() {
}

void Renderer::run() {
    mainLoop();
}

void Renderer::mainLoop() {
    bool fpsCounted = true;
    Clock clock;
    int frames = 0;
    while ( !mContext.window().shouldClose() ) {
        glfwPollEvents();
        if (fpsCounted) {
            clock.start();
            fpsCounted = false;
        }
        drawFrame();
        ++frames;
        clock.stop();
        if (clock.duration() > 0.1) {
            std::string title = "FPS: " + std::to_string(frames / clock.duration());
            glfwSetWindowTitle(mContext.window().window(), title.c_str());
            fpsCounted = true;
            frames = 0;
        }
    }
    vkDeviceWaitIdle( mContext.device() );
}

void Renderer::drawFrame() {
    uint32_t imageIndex;
    auto acquireResult = mSwapChain.acquireNextImage(&imageIndex,
                                 mSyncObjects.imageAvailableSemaphore(mSwapChain.currentFrame()),
                                 mSyncObjects.inFlightFence(mSwapChain.currentFrame()));

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR ) {
        recreateSwapChain();
        return;
    } else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    // Only reset the fence if we are submitting work
    vkResetFences(mContext.device(), 1, &mSyncObjects.inFlightFence(mSwapChain.currentFrame()));

    mUniformBuffers.updateUniformBuffer(mSwapChain.currentFrame(), mSwapChain.extent());

    mCommandManager.recordCommandBuffer( &mSwapChain, &mGraphicsPipeline, &mDescriptorSet, &mVertexBuffer, imageIndex );
    auto submitResult = mCommandManager.submitCommandBuffer( &mSwapChain, &mSyncObjects, &imageIndex );
    if (submitResult == VK_ERROR_OUT_OF_DATE_KHR ||
            submitResult == VK_SUBOPTIMAL_KHR || mContext.window().frameBufferResized()) {
        mContext.window().setResized(false);
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
    mDepthResources.recreate(mSwapChain.extent());
    mSwapChain.createFrameBuffers(mGraphicsPipeline.renderPass(), mDepthResources.imageView());
}