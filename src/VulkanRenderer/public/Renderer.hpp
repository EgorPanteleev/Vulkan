//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "Camera.hpp"
#include "GraphicsPipeline.hpp"
#include "CommandManager.hpp"
#include "SyncObjects.hpp"
#include "UniformBuffer.hpp"
#include "Texture.hpp"
#include "ShadowDescriptorSet.hpp"
#include "ShadowPipeline.hpp"
#include "VulkanModelLoader.hpp"
#include "VkImGui.hpp"

class Renderer {
public:
    Renderer(const std::string& modelPath, CameraCreateInfo& cameraCreateInfo);
    ~Renderer();

    void run();
    void quit();

    Context* context() { return mContext.get(); }
    Camera* camera() { return mCamera.get(); }
    Window* window() { return &mContext->window(); }

    void setImGuiUsage(bool use) { mImGuiUsage = use; }
    bool imGuiUsage() const { return mImGuiUsage; }

    void setKeyboardCallBack(void (*processKeyboard)(GLFWwindow* window, Camera* camera, double deltaTime)) { mProcessKeyboard = processKeyboard; }
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

    /// Render implementation
    UniquePtr<Camera> mCamera;
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
    void (*mProcessKeyboard)(GLFWwindow* window, Camera* camera, double deltaTime);
};


#endif //VULKAN_RENDERER_H
