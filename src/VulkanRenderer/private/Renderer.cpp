//
// Created by auser on 4/4/25.
//

#include "Renderer.hpp"
#include "Timer.hpp"
#include "Message.hpp"
#include "ModelUniformBuffer.hpp"
#include "LightUniformBuffer.hpp"
#include "DirectionalLightBuffer.hpp"
#include "Utils.hpp"
#include <backends/imgui_impl_vulkan.h>


Renderer::Renderer(const std::string& modelPath, CameraCreateInfo& cameraCreateInfo): mCurrentFrame(0) {
    mCamera = std::make_unique<Camera>(cameraCreateInfo);
    ContextCreateInfo contextCreateInfo {
        .maxFramesInFlight = 3,
        .validationLayers = { "VK_LAYER_KHRONOS_validation" },
        .deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                              VK_KHR_MAINTENANCE_1_EXTENSION_NAME },
#ifdef NDEBUG
        .enableValidationLayers = false,
#else
        .enableValidationLayers = true,
#endif
    };
    mContext = std::make_unique<Context>(contextCreateInfo);
    mLoader = std::make_unique<VulkanModelLoader>(mContext.get(), modelPath);
    if (!mLoader->load()) exit(-1);

    mSwapChain = std::make_unique<SwapChain>(mContext.get());
    mUniformBuffers = std::make_unique<UniformBuffers>();
    mUniformBuffers->emplace_back(std::make_unique<ModelUniformBuffer>(mContext.get(), mCamera.get()));
    mUniformBuffers->emplace_back(std::make_unique<LightUniformBuffer>(mContext.get(), mCamera.get()));
    mUniformBuffers->emplace_back(std::make_unique<DirectionalLightBuffer>(mContext.get(), mCamera.get(),
                                                                           mLoader->bbox(), glm::vec3(0.0f, -2.0f, -0.4f)));

    loadShader(COMPILED_SHADERS_PATH"shadowShader.vert.spv", mShadowVertShaderModule);
    ShadowPipelineCreateInfo shadowPipelineCreateInfo {
            .context = mContext.get(),
            .uniformBuffers = *mUniformBuffers,
            .vertShaderModule = mShadowVertShaderModule,
            .extent = {2048, 2048}
    };
    mShadowPipeline = std::make_unique<ShadowPipeline>(shadowPipelineCreateInfo);

    loadShader(COMPILED_SHADERS_PATH"shader.vert.spv", mMainVertShaderModule);
    loadShader(COMPILED_SHADERS_PATH"shader.frag.spv", mFragShaderModule);
    GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{
            .context = mContext.get(),
            .swapChain = mSwapChain.get(),
            .loader = mLoader.get(),
            .shadowMap = mShadowPipeline->shadowMap(),
            .uniformBuffers = *mUniformBuffers,
            .vertShaderModule = mMainVertShaderModule,
            .fragShaderModule = mFragShaderModule
    };
    mGraphicsPipeline = std::make_unique<GraphicsPipeline>(graphicsPipelineCreateInfo);
    mCommandManager = std::make_unique<CommandManager>(mContext.get());
    mVertexBuffer = std::make_unique<VertexBuffer>(mContext.get(), mLoader.get());
    mSyncObjects = std::make_unique<SyncObjects>(mContext.get(), mSwapChain.get());

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

void Renderer::quit() {
    glfwSetWindowShouldClose(mContext->glfwWindow(), GLFW_TRUE);
}

void Renderer::mainLoop() {
    FpsCounter fpsCounter;
    double deltaTime = 0;
    while ( !mContext->window().shouldClose() ) {
        glfwPollEvents();
        beginFrame();
        render();
        endFrame();
        fpsCounter.update();
        deltaTime = 1e3 / fpsCounter.fps();
        processKeyboard(deltaTime);
        glfwSetWindowTitle(mContext->window().window(), std::to_string(fpsCounter.fps()).c_str());
    }
    vkDeviceWaitIdle( mContext->device() );
}

glm::vec3 dir = glm::vec3(0.0f, -1.0f, -0.2f);

void Renderer::beginFrame() {
    auto acquireResult = mSwapChain->acquireNextImage(mSyncObjects->imageAvailableSemaphore(mCurrentFrame),
                                                               mSyncObjects->inFlightFence(mCurrentFrame));

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR ) {
        recreateSwapChain();
        return;
    } else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

}

void Renderer::endFrame() {
    VkImGui* gui = nullptr;
    if ( mImGuiUsage ) gui = mVkImGui.get();
    CommandManagerRecordInfo commandManagerRecordInfo{
            .swapChain = mSwapChain.get(),
            .graphicsPipeline = mGraphicsPipeline.get(),
            .shadowPipeline = mShadowPipeline.get(),
            .vkImGui = gui,
            .vertexBuffer = mVertexBuffer.get(),
            .imageIndex = mSwapChain->imageIndex(),
            .currentFrame = mCurrentFrame
    };
    mCommandManager->recordCommandBuffer(commandManagerRecordInfo);

    vkResetFences(mContext->device(), 1, &mSyncObjects->inFlightFence(mCurrentFrame));

    CommandManagerSubmitInfo commandManagerSubmitInfo{
            .swapChain = mSwapChain.get(),
            .syncObjects = mSyncObjects.get(),
            .imageIndex = mSwapChain->imageIndex(),
            .currentFrame = mCurrentFrame
    };
    auto submitResult = mCommandManager->submitCommandBuffer(commandManagerSubmitInfo);

    if (submitResult == VK_ERROR_OUT_OF_DATE_KHR ||
        submitResult == VK_SUBOPTIMAL_KHR || mContext->window().frameBufferResized()) {
        mContext->window().setResized(false);
        recreateSwapChain();
        return;
    } else if (submitResult != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit command buffer!");
    }
    updateCurrentFrame();
}

void Renderer::render() {
    ((DirectionalLightBuffer*)((*mUniformBuffers)[2].get()))->setDirection(glm::normalize(dir));
    for ( auto& uniformBuffer: *mUniformBuffers ) {
        uniformBuffer->updateUniformBuffer(mCurrentFrame, mSwapChain->extent() );
    }

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.4);
    mVkImGui->beginFrame();
    if ( mImGuiUsage ) {
        ImGui::Begin("Settings");
        ImVec2 mousePos = ImGui::GetMousePos();
        ImGui::Text("Mouse pos: %.1f x %.1f", mousePos.x, mousePos.y);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        ImGui::Separator();
        ImGui::DragFloat3("Light direction", &dir.x, 0.005f, -1.0f, 1.0f);

        ImGui::End();
    }
    mVkImGui->endFrame();
}

void Renderer::recreateSwapChain() {
    vkDeviceWaitIdle(mContext->device());
    mSwapChain->clear();
    mSwapChain->recreate();
    mGraphicsPipeline->recreateBuffers();
    mGraphicsPipeline->updateDescriptorSet();
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