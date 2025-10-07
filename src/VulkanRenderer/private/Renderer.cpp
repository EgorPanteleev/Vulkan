//
// Created by auser on 4/4/25.
//

#include <backends/imgui_impl_vulkan.h>
#include <tracy/Tracy.hpp>

#include "Renderer.hpp"
#include "Timer.hpp"
#include "Message.hpp"
#include "ModelUniformBuffer.hpp"
#include "LightUniformBuffer.hpp"
#include "DirectionalLightBuffer.hpp"

static UiState uiState{};

Renderer::Renderer(const std::string& modelPath, const std::vector<std::string>& skyBoxPaths,
                   const cs::CameraCreateInfo& cameraCreateInfo): mCurrentFrame(0) {
    uiState.camType = cameraCreateInfo.type;
    cs::CameraCreateInfo camCreateInfo = cameraCreateInfo;
    camCreateInfo.type = cs::CameraType::FLY;
    mFlyCamera = makeCameraUnique(camCreateInfo);
    camCreateInfo.type = cs::CameraType::ORBITAL;
    mOrbitalCamera = makeCameraUnique(camCreateInfo);

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
    mLoader = std::make_unique<VulkanModelLoader>(mContext.get(), modelPath, skyBoxPaths);
    if (!mLoader->load()) exit(-1);

    mSwapChain = std::make_unique<SwapChain>(mContext.get());
    mUniformBuffers = std::make_unique<UniformBuffers>();
    mUniformBuffers->emplace_back(std::make_unique<ModelUniformBuffer>(mContext.get(), camera()));
    mUniformBuffers->emplace_back(std::make_unique<LightUniformBuffer>(mContext.get(), camera()));
    mUniformBuffers->emplace_back(std::make_unique<DirectionalLightBuffer>(mContext.get(), camera(),
                                                                           mLoader->bbox(), glm::vec3(0.0f, -2.0f, -0.4f)));

    ShadowPipelineCreateInfo shadowPipelineCreateInfo {
            .context = mContext.get(),
            .uniformBuffers = *mUniformBuffers,
            .vertShaderPath = COMPILED_SHADERS_PATH"shadowShader.vert.spv",
            .extent = {2048, 2048}
    };
    mShadowPipeline = std::make_unique<ShadowPipeline>(shadowPipelineCreateInfo);

    createGraphicsPipeline();
    createCubeMapPipeline();
    mCommandManager = std::make_unique<CommandManager>(mContext.get());
    mVertexBuffer = std::make_unique<VertexBuffer<Vertex>>(mContext.get(), mLoader->vulkanVertices(), mLoader->indices());
    std::vector<glm::vec3> skyboxVertices = {
            {-1.0f, -1.0f, -1.0f}, // 0
            { 1.0f, -1.0f, -1.0f}, // 1
            { 1.0f,  1.0f, -1.0f}, // 2
            {-1.0f,  1.0f, -1.0f}, // 3
            {-1.0f, -1.0f,  1.0f}, // 4
            { 1.0f, -1.0f,  1.0f}, // 5
            { 1.0f,  1.0f,  1.0f}, // 6
            {-1.0f,  1.0f,  1.0f}  // 7
    };
    std::vector<uint32_t> skyboxIndices = {
            // back face
            0, 1, 2, 2, 3, 0,
            // front face
            4, 5, 6, 6, 7, 4,
            // left face
            4, 0, 3, 3, 7, 4,
            // right face
            1, 5, 6, 6, 2, 1,
            // bottom face
            4, 5, 1, 1, 0, 4,
            // top face
            3, 2, 6, 6, 7, 3
    };
    mSkyBoxVertexBuffer = std::make_unique<VertexBuffer<glm::vec3>>(mContext.get(), skyboxVertices, skyboxIndices);
    mSyncObjects = std::make_unique<SyncObjects>(mContext.get(), mSwapChain.get());

    mVkImGui = std::make_unique<VkImGui>(mContext.get(), mSwapChain.get());
    mSwapChain->createImGuiFrameBuffers(mVkImGui->renderPass());
    mImGuiUsage = false;
}

Renderer::~Renderer() {
}

void Renderer::run() {
    mainLoop();
}

void Renderer::quit() {
    glfwSetWindowShouldClose(mContext->glfwWindow(), GLFW_TRUE);
}

cs::AbsCamera* Renderer::camera() {
    switch (uiState.camType) {
        case cs::CameraType::FLY:
            return mFlyCamera.get();
        case cs::CameraType::ORBITAL:
            return mOrbitalCamera.get();
        default:
            throw std::runtime_error("Unsupported camera type!");
    }
}

void Renderer::mainLoop() {
    FpsCounter fpsCounter;
    double deltaTime = 0;
    while ( !mContext->window().shouldClose() ) {
        ZoneScopedN("MainLoop");
        glfwPollEvents();
        beginFrame();
        render();
        endFrame();
        fpsCounter.update();
        deltaTime = 1e3 / fpsCounter.fps();
        processKeyboard(deltaTime);
        glfwSetWindowTitle(mContext->window().window(), std::to_string(fpsCounter.fps()).c_str());
        FrameMark;
    }
    vkDeviceWaitIdle( mContext->device() );
}

void Renderer::beginFrame() {
    ZoneScopedN("Begin frame");
    VkResult acquireResult = mSwapChain->acquireNextImage(mSyncObjects->imageAvailableSemaphore(mCurrentFrame),
                                                          mSyncObjects->inFlightFence(mCurrentFrame));
    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR ) {
        recreateSwapChain();
        return;
    } else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }
}

void Renderer::endFrame() {
    ZoneScopedN("End frame");
    VkImGui* gui = nullptr;
    if ( mImGuiUsage ) gui = mVkImGui.get();
    CommandManagerRecordInfo commandManagerRecordInfo {
            .swapChain = mSwapChain.get(),
            .graphicsPipeline = mGraphicsPipeline.get(),
            .shadowPipeline = mShadowPipeline.get(),
            .cubeMapPipeline = mCubeMapPipeline.get(),
            .vkImGui = gui,
            .vertexBuffer = mVertexBuffer.get(),
            .skyBoxVertexBuffer = mSkyBoxVertexBuffer.get(),
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
    ZoneScopedN("Render frame");
    ((DirectionalLightBuffer*)((*mUniformBuffers)[2].get()))->setDirection(glm::normalize(uiState.lightDir));
    for ( auto& uniformBuffer: *mUniformBuffers ) {
        uniformBuffer->updateUniformBuffer(mCurrentFrame, mSwapChain->extent() );
    }

    mVkImGui->beginFrame();
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 115), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.4);
    if ( mImGuiUsage ) {
        bool isFlyCamera = uiState.camType == cs::CameraType::FLY;

        ImGui::Begin("Settings");
        ImVec2 mousePos = ImGui::GetMousePos();
        ImGui::Text("Mouse pos: %.1f x %.1f", mousePos.x, mousePos.y);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        ImGui::Separator();
        ImGui::DragFloat3("Light direction", &uiState.lightDir.x, 0.005f, -1.0f, 1.0f);

        ImGui::Separator();
        if (VkImGui::selectableButton("Fly", isFlyCamera)) {
            uiState.camType = cs::CameraType::FLY;
            updateCameras();
        }
        ImGui::SameLine(0.0f, 5.0f);
        if (VkImGui::selectableButton("Orbital", !isFlyCamera)) {
            uiState.camType = cs::CameraType::ORBITAL;
            updateCameras();
        }
        ImGui::SameLine();
        ImGui::Text("Camera type");
        ImGui::Separator();
        if (ImGui::Checkbox("MSAA", &uiState.enableMSAA)) {
            createGraphicsPipeline();
        }
        ImGui::End();
    }
    mVkImGui->endFrame();
}

void Renderer::recreateSwapChain() {
    ZoneScopedN("Recreate swapchain");
    vkDeviceWaitIdle(mContext->device());
    mSwapChain->clear();
    mSwapChain->recreate();
    mGraphicsPipeline->recreateBuffers();
    mGraphicsPipeline->updateDescriptorSet();
    mSwapChain->createImGuiFrameBuffers(mVkImGui->renderPass());

    INFO << "Swapchain recreated!";
}

void Renderer::processKeyboard(double deltaTime) {
    if (mProcessKeyboard)
        mProcessKeyboard(mContext->glfwWindow(), camera(), deltaTime);
}

void Renderer::updateCameras() {
    mFlyCamera->setPosition(mOrbitalCamera->position());
    mFlyCamera->setOrientation(mOrbitalCamera->orientation());
    for (auto& uniformBuffer: *mUniformBuffers.get()) {
        uniformBuffer->setCamera(camera());
    }
}

void Renderer::createGraphicsPipeline() {
    GraphicsPipelineCreateInfo graphicsPipelineCreateInfo {
            .context = mContext.get(),
            .swapChain = mSwapChain.get(),
            .loader = mLoader.get(),
            .shadowMap = mShadowPipeline->shadowMap(),
            .uniformBuffers = *mUniformBuffers,
            .vertShaderPath = COMPILED_SHADERS_PATH"shader.vert.spv",
            .fragShaderPath = COMPILED_SHADERS_PATH"shader.frag.spv",
            .enableMSAA = uiState.enableMSAA,
    };
    mGraphicsPipeline = std::make_unique<GraphicsPipeline>(graphicsPipelineCreateInfo);
}

void Renderer::createCubeMapPipeline() {
    CubeMapPipelineCreateInfo cubeMapPipelineCreateInfo {
        .context = mContext.get(),
        .swapChain = mSwapChain.get(),
        .loader = mLoader.get(),
        .uniformBuffers = *mUniformBuffers,
        .vertShaderPath = COMPILED_SHADERS_PATH"cubeMapShader.vert.spv",
        .fragShaderPath = COMPILED_SHADERS_PATH"cubeMapShader.frag.spv",
    };
    mCubeMapPipeline = std::make_unique<CubeMapPipeline>(cubeMapPipelineCreateInfo);
}
