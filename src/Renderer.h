//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "GraphicsPipeline.h"
#include "CommandManager.h"
#include "SyncObjects.h"
#include "UniformBuffers.h"
#include "Image.h"
#include "DepthResources.h"
#include "ColorResources.h"

//#include <memory>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void run();
private:
    void mainLoop();
    void drawFrame();
    void recreateSwapChain();

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
