//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "Camera.h"
#include "GraphicsPipeline.h"
#include "CommandManager.h"
#include "SyncObjects.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "DepthResources.h"
#include "ColorResources.h"
#include "ShadowDescriptorSet.h"
#include "ShadowPipeline.h"
#include "VulkanModelLoader.h"
#include "VkImGui.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    void run();

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
    void beginFrame(uint32_t& imageIndex);
    void endFrame(uint32_t& imageIndex);
    void render();
    void recreateSwapChain();
    void loadShader(const std::string& shaderPath, VkShaderModule& module);
    void processKeyboard(double deltaTime);
    void updateCurrentFrame() { mCurrentFrame = (mCurrentFrame + 1) % mContext->maxFramesInFlight(); }

    UniquePtr<Camera> mCamera;
    UniquePtr<Context> mContext;
    UniquePtr<VulkanModelLoader> mLoader;
    UniquePtr<Texture> mTexture;
    UniquePtr<SwapChain> mSwapChain;
    UniquePtr<ColorResources> mColorResources;
    UniquePtr<DepthResources> mDepthResources;
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

    uint32_t mCurrentFrame;
    bool mImGuiUsage;
    void (*mProcessKeyboard)(GLFWwindow* window, Camera* camera, double deltaTime);
};


#endif //VULKAN_RENDERER_H
