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
#include "Image.h"
#include "DepthResources.h"
#include "ColorResources.h"
#include "ShadowDescriptorSet.h"
#include "ShadowPipeline.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    void run();

    Context* context() { return mContext.get(); }
    Camera* camera() {return mCamera.get(); }
private:
    using UniformBuffers = std::vector<std::unique_ptr<UniformBuffer>>;
    template <typename Type>
    using UniquePtr = std::unique_ptr<Type>;

    void mainLoop();
    void drawFrame();
    void recreateSwapChain();
    void loadShader(const std::string& shaderPath, VkShaderModule& module);

    UniquePtr<Camera> mCamera;
    UniquePtr<Context> mContext;
    UniquePtr<Image> mTexture;
    UniquePtr<SwapChain> mSwapChain;
    UniquePtr<ColorResources> mColorResources;
    UniquePtr<DepthResources> mDepthResources;
    UniquePtr<UniformBuffers> mUniformBuffers;
    UniquePtr<ShadowDescriptorSet> mShadowDescriptorSet;
    UniquePtr<ShadowPipeline> mShadowPipeline;
    UniquePtr<DescriptorSet> mDescriptorSet;
    UniquePtr<GraphicsPipeline> mGraphicsPipeline;
    UniquePtr<CommandManager> mCommandManager;
    UniquePtr<VertexBuffer> mVertexBuffer;
    UniquePtr<SyncObjects> mSyncObjects;

    VkShaderModule mShadowVertShaderModule;

    VkShaderModule mMainVertShaderModule;
    VkShaderModule mFragShaderModule;
};


#endif //VULKAN_RENDERER_H
