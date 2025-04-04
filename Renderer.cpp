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
    mSwapChain.acquireNextImage(&imageIndex,
                                 mSyncObjects.imageAvailableSemaphore(mSwapChain.currentFrame()),
                                 mSyncObjects.inFlightFence(mSwapChain.currentFrame()));
    mCommandManager.recordCommandBuffer( &mSwapChain, &mGraphicsPipeline, imageIndex );
    mCommandManager.submitCommandBuffer( &mSwapChain, &mSyncObjects, &imageIndex );
    mSwapChain.updateCurrentFrame();
}