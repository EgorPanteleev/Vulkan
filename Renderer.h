//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "GraphicsPipeline.h"
#include "CommandManager.h"
#include "SyncObjects.h"

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
    SwapChain mSwapChain;
    GraphicsPipeline mGraphicsPipeline;
    CommandManager mCommandManager;
    SyncObjects mSyncObjects;
};


#endif //VULKAN_RENDERER_H
