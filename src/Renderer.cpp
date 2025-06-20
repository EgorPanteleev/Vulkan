//
// Created by auser on 4/4/25.
//

#include "Renderer.h"
#include "Timer.h"
#include "MessageLogger.h"
#include "ModelUniformBuffer.h"
#include "LightUniformBuffer.h"
#include "DirectionalLightBuffer.h"
#include "Utils.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
//#define MODEL_PATH PROJECT_PATH"models/Bistro/BistroExterior.fbx"
//#define MODEL_PATH PROJECT_PATH"models/dragon/scene.obj"
//#define MODEL_PATH PROJECT_PATH"models/Sponza/sponza.obj"
#define MODEL_PATH PROJECT_PATH"models/Sponza/glTF/Sponza.gltf"

#define TEXTURE_PATH PROJECT_PATH"textures/statue.jpg"

#define COMPILED_SHADERS_PATH BINARY_PATH"compiled_shaders/"

//static std::string MODEL_PATH =
//        "/home/auser/dev/src/Vulkan/models/lamborghini/source/2022_Lamborghini_Countach_LPI_800-4_LBWK_3D_Assetto/ksp_lambo_countach_lbwk/lambo_countach_2022.obj";
//static std::string TEXTURE_PATH =
//        "/home/auser/dev/src/Vulkan/models/lamborghini/textures/Countach50_01_InteriorA_.png";

Renderer::Renderer() {
    glm::vec3 camPos(0, 0, 0);
    glm::vec3 camTarget(-1, 0, 0);
    glm::vec3 up(0, 1, 0);
    float FOV         = 45;
    float aspectRatio = 1920.0f / 1200.0f;
    float nearPlane   = 0.1f;
    float farPlane    = 10000.f;
    mCamera = std::make_unique<Camera>(camPos, camTarget, up,
                                       FOV, aspectRatio, nearPlane, farPlane);
    mContext = std::make_unique<Context>();
    mLoader = std::make_unique<VulkanModelLoader>(mContext.get(), MODEL_PATH);
    if (!mLoader->load()) exit(-1);

    mTexture = std::make_unique<Texture>(mContext.get(), true);
    mTexture->load(TEXTURE_PATH);
    mSwapChain = std::make_unique<SwapChain>(mContext.get());
    mColorResources = std::make_unique<ColorResources>(mContext.get(), mSwapChain->extent(), mSwapChain->format());
    mDepthResources = std::make_unique<DepthResources>(mContext.get(), mSwapChain->extent());
    mUniformBuffers = std::make_unique<UniformBuffers>();
    mUniformBuffers->emplace_back(std::make_unique<ModelUniformBuffer>(mContext.get(), mCamera.get()));
    mUniformBuffers->emplace_back(std::make_unique<LightUniformBuffer>(mContext.get(), mCamera.get()));
    mUniformBuffers->emplace_back(std::make_unique<DirectionalLightBuffer>(mContext.get(), mCamera.get(),
                                                                           mLoader->bbox(), glm::vec3(0.0f, -2.0f, -0.4f)));

    mShadowDescriptorSet = std::make_unique<ShadowDescriptorSet>(mContext.get(), mDepthResources.get(), *mUniformBuffers);
    loadShader(COMPILED_SHADERS_PATH"shadowShader.vert.spv", mShadowVertShaderModule);
    mShadowPipeline = std::make_unique<ShadowPipeline>(mContext.get(), mShadowDescriptorSet.get(), mShadowVertShaderModule);

    mDescriptorSet = std::make_unique<DescriptorSet>(mContext.get(), mTexture.get(), mLoader.get(), mDepthResources.get(), *mUniformBuffers);
    loadShader(COMPILED_SHADERS_PATH"shader.vert.spv", mMainVertShaderModule);
    loadShader(COMPILED_SHADERS_PATH"shader.frag.spv", mFragShaderModule);
    mGraphicsPipeline = std::make_unique<GraphicsPipeline>(mContext.get(), mSwapChain.get(), mDescriptorSet.get(),
                                                           mMainVertShaderModule, mFragShaderModule);
    mCommandManager = std::make_unique<CommandManager>(mContext.get(), mDepthResources.get());
    mVertexBuffer = std::make_unique<VertexBuffer>(mContext.get(), mLoader.get());
    mSyncObjects = std::make_unique<SyncObjects>(mContext.get(), mSwapChain.get());


    mSwapChain->createShadowFrameBuffers(mShadowPipeline->renderPass(), mDepthResources->shadowImageView());
    mSwapChain->createFrameBuffers(mGraphicsPipeline->renderPass(),
                                   mDepthResources->imageView(),
                                   mColorResources->imageView());

    mVkImGui = std::make_unique<VkImGui>(mContext.get(), mSwapChain.get());
    mSwapChain->createImGuiFrameBuffers(mVkImGui->renderPass());
    mImGuiUsage = false;
}

Renderer::~Renderer() {
    vkDestroyShaderModule( mContext->device(), mShadowVertShaderModule, nullptr );
    vkDestroyShaderModule( mContext->device(), mMainVertShaderModule, nullptr );
    vkDestroyShaderModule( mContext->device(), mFragShaderModule, nullptr );
}

void Renderer::run() {
    mainLoop();
}

void Renderer::mainLoop() {
    FpsCounter fpsCounter;
    double deltaTime = 0;
    while ( !mContext->window().shouldClose() ) {
        glfwPollEvents();
        drawFrame();
        fpsCounter.update();
        deltaTime = 1e3 / fpsCounter.fps();
//        INFO << deltaTime;
        processKeyboard(deltaTime);
        glfwSetWindowTitle(mContext->window().window(), std::to_string(fpsCounter.fps()).c_str());
    }
    vkDeviceWaitIdle( mContext->device() );
}

void Renderer::drawFrame() {
    uint32_t imageIndex;
    auto acquireResult = mSwapChain->acquireNextImage(&imageIndex,
                                 mSyncObjects->imageAvailableSemaphore(mSwapChain->currentFrame()),
                                 mSyncObjects->inFlightFence(mSwapChain->currentFrame()));

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR ) {
        recreateSwapChain();
        return;
    } else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    // Only reset the fence if we are submitting work
    vkResetFences(mContext->device(), 1, &mSyncObjects->inFlightFence(mSwapChain->currentFrame()));

    for ( auto& uniformBuffer: *mUniformBuffers ) {
        uniformBuffer->updateUniformBuffer(mSwapChain->currentFrame(), mSwapChain->extent() );
    }

    VkImGui* gui = nullptr;
    if ( mImGuiUsage ) {
        gui = mVkImGui.get();
        mVkImGui->beginFrame();
        //VkImGui::demo();

        ImGui::Text("asd");

        mVkImGui->endFrame();
    }

    mCommandManager->recordCommandBuffer( mSwapChain.get(), mGraphicsPipeline.get(), mShadowPipeline.get(),
                                         mDescriptorSet.get(), mShadowDescriptorSet.get(), mVertexBuffer.get(), imageIndex, gui);
    auto submitResult = mCommandManager->submitCommandBuffer( mSwapChain.get(), mSyncObjects.get(), &imageIndex );

    if (submitResult == VK_ERROR_OUT_OF_DATE_KHR ||
            submitResult == VK_SUBOPTIMAL_KHR || mContext->window().frameBufferResized()) {
        mContext->window().setResized(false);
        recreateSwapChain();
        return;
    } else if (submitResult != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit command buffer!");
    }
    mSwapChain->updateCurrentFrame();
}

void Renderer::recreateSwapChain() {
    vkDeviceWaitIdle(mContext->device());
    mSwapChain->clear();
    mSwapChain->recreate();
    mColorResources->recreate(mSwapChain->extent(), mSwapChain->format());
    mDepthResources->recreate(mSwapChain->extent());
    mSwapChain->createFrameBuffers(mGraphicsPipeline->renderPass(),
                                   mDepthResources->imageView(), mColorResources->imageView());
    mSwapChain->createShadowFrameBuffers(mShadowPipeline->renderPass(), mDepthResources->shadowImageView());
    mSwapChain->createImGuiFrameBuffers(mVkImGui->renderPass());

    INFO << "Swapchain recreated!";
}

void Renderer::loadShader(const std::string& shaderPath, VkShaderModule& module) {
    auto shaderCode = Utils::readFile(shaderPath);
    Utils::createShaderModule(mContext->device(), shaderCode, &module);
}

void Renderer::processKeyboard(double deltaTime) {
    if (mProcessKeyboard)
        mProcessKeyboard(mContext->glfwWindow(), mCamera.get(), deltaTime);
}