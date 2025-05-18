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

//#include <memory>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void run();

    Context* context() { return &mContext; }
    Camera* camera() {return &mCamera; }
private:
    using UniformBuffers = std::vector<std::unique_ptr<UniformBuffer>>;

    void mainLoop();
    void drawFrame();
    void recreateSwapChain();

    Camera mCamera;
    Context mContext;
    Image image;
    SwapChain mSwapChain;
    ColorResources mColorResources;
    DepthResources mDepthResources;
    UniformBuffers mUniformBuffers;
    DescriptorSet mDescriptorSet;
    GraphicsPipeline mGraphicsPipeline;
    CommandManager mCommandManager;
    VertexBuffer mVertexBuffer;
    SyncObjects mSyncObjects;
};


#endif //VULKAN_RENDERER_H
