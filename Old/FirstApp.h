//
// Created by auser on 3/21/25.
//
#ifndef FIRSTAPP_H
#define FIRSTAPP_H

#include "../Window.h"
#include "Device.h"
#include "SwapChain_old.h"
#include "../GraphicsPipeline.h"
#include "RenderPass.h"
#include "FrameBuffers.h"
#include "CommandBuffers.h"
#include "../SyncObjects.h"

#include <vector>
#include <memory>

class FirstApp {
public:
    FirstApp();

    void run();

    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    static constexpr std::string NAME = "VulkanApp";
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
private:

    void initVulkan();

    void mainLoop();

    void cleanup();

    void drawFrame();

    Window window; /// glfw window
    Device device;
    SyncObjects syncObjects;
    std::unique_ptr<SwapChain_old> swapChain;
    RenderPass renderPass;
    std::unique_ptr<GraphicsPipeline> graphicsPipeline;
    FrameBuffers frameBuffers;
    CommandBuffers commandBuffers;
};


#endif //FIRSTAPP_H
