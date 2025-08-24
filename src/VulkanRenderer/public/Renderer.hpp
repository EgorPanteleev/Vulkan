//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "AbsCamera.hpp"
#include "GraphicsPipeline.hpp"
#include "CommandManager.hpp"
#include "SyncObjects.hpp"
#include "UniformBuffer.hpp"
#include "Texture.hpp"
#include "ShadowDescriptorSet.hpp"
#include "ShadowPipeline.hpp"
#include "VulkanModelLoader.hpp"
#include "VkImGui.hpp"

struct UiState {
    glm::vec3 lightDir = {0.0f, -1.0f, -0.2f};
    CameraType camType = CameraType::FLY;
    bool enableMSAA = false;
};

class Renderer {
public:
    Renderer(const std::string& modelPath, const CameraCreateInfo& cameraCreateInfo);
    ~Renderer();

    void run();
    void quit();

    Context* context() { return mContext.get(); }
    AbsCamera* camera();
    Window* window() { return &mContext->window(); }

    void setImGuiUsage(bool use) { mImGuiUsage = use; }
    bool imGuiUsage() const { return mImGuiUsage; }

    void setKeyboardCallBack(void (*processKeyboard)(GLFWwindow* window, AbsCamera* camera, double deltaTime)) { mProcessKeyboard = processKeyboard; }
private:
    using UniformBuffers = std::vector<std::unique_ptr<UniformBuffer>>;
    template <typename Type>
    using UniquePtr = std::unique_ptr<Type>;

    void mainLoop();
    void beginFrame();
    void endFrame();
    void render();
    void recreateSwapChain();
    void loadShader(const std::string& shaderPath, VkShaderModule& module);
    void processKeyboard(double deltaTime);
    void updateCurrentFrame() { mCurrentFrame = (mCurrentFrame + 1) % mContext->maxFramesInFlight(); }
    void updateCameras();
    void createGraphicsPipeline();

    /// Render implementation
    UniquePtr<AbsCamera> mFlyCamera;
    UniquePtr<AbsCamera> mOrbitalCamera;
    UniquePtr<Context> mContext;
    UniquePtr<VulkanModelLoader> mLoader;
    UniquePtr<SwapChain> mSwapChain;
    UniquePtr<UniformBuffers> mUniformBuffers;
    UniquePtr<ShadowDescriptorSet> mShadowDescriptorSet;
    UniquePtr<ShadowPipeline> mShadowPipeline;
    UniquePtr<GraphicsPipeline> mGraphicsPipeline;
    UniquePtr<CommandManager> mCommandManager;
    UniquePtr<VertexBuffer> mVertexBuffer;
    UniquePtr<SyncObjects> mSyncObjects;
    UniquePtr<VkImGui> mVkImGui;
    VkShaderModule mShadowVertShaderModule;
    VkShaderModule mMainVertShaderModule;
    VkShaderModule mFragShaderModule;

    /// Temporary information
    bool mImGuiUsage;
    uint32_t mCurrentFrame;

    /// CallBacks
    void (*mProcessKeyboard)(GLFWwindow* window, AbsCamera* camera, double deltaTime);
};


#endif //VULKAN_RENDERER_H
