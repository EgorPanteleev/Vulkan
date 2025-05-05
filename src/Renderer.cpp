//
// Created by auser on 4/4/25.
//

#include "Renderer.h"
#include "Clock.h"
#include "MessageLogger.h"

static std::string MODEL_PATH =
        "/home/auser/dev/src/Vulkan/models/woodspring-priory/source/WoodspringPriory/WoodspringPriory.obj";
static std::string TEXTURE_PATH =
        "/home/auser/dev/src/Vulkan/models/woodspring-priory/source/WoodspringPriory/WoodspringPrioryExterior01_Model_4a_u1_v1_diffuse.jpg";

//static std::string MODEL_PATH =
//        "/home/auser/dev/src/Vulkan/models/lamborghini/source/2022_Lamborghini_Countach_LPI_800-4_LBWK_3D_Assetto/ksp_lambo_countach_lbwk/lambo_countach_2022.obj";
//static std::string TEXTURE_PATH =
//        "/home/auser/dev/src/Vulkan/models/lamborghini/textures/Countach50_01_InteriorA_.png";

Renderer::Renderer(): mCamera(),
                      mContext(), image(&mContext, TEXTURE_PATH),
                      mSwapChain(&mContext), mColorResources(&mContext, mSwapChain.extent(), mSwapChain.format()),
                      mDepthResources(&mContext, mSwapChain.extent()), mUniformBuffers(&mContext, &mCamera),
                      mDescriptorSet(&mContext, &image, &mUniformBuffers),
                      mGraphicsPipeline(&mContext, &mSwapChain, &mDescriptorSet,
                                        "/home/auser/dev/src/Vulkan/compiled_shaders/shader.vert.spv",
                                        "/home/auser/dev/src/Vulkan/compiled_shaders/shader.frag.spv"),
                      mCommandManager(&mContext),
                      mVertexBuffer(&mContext, MODEL_PATH), mSyncObjects(&mContext) {
    mSwapChain.createFrameBuffers(mGraphicsPipeline.renderPass(), mDepthResources.imageView(), mColorResources.imageView());

    glm::vec3 min(FLT_MAX);
    glm::vec3 max(-FLT_MAX);

    for (const auto& vertex : mVertexBuffer.vertices()) {
        min = glm::min(min, vertex.pos);
        max = glm::max(max, vertex.pos);
    }

    glm::vec3 center = (min + max) * 0.5f;
    glm::vec3 size = max - min;
    float radius = glm::length(size) * 0.5f;
    float fov = glm::radians(mCamera.zoom()); // or glm::radians(45.0f)
    float distance = radius / std::tan(fov / 2.0f);
    mCamera.setTarget(center);
    mCamera.setPosition(center + glm::vec3(distance));
    mCamera.updatePosition();

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

    mUniformBuffers.updateUniformBuffer(mSwapChain.currentFrame(), mSwapChain.extent() );

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
    mColorResources.recreate(mSwapChain.extent(), mSwapChain.format());
    mDepthResources.recreate(mSwapChain.extent());
    mSwapChain.createFrameBuffers(mGraphicsPipeline.renderPass(), mDepthResources.imageView(), mColorResources.imageView());
}
