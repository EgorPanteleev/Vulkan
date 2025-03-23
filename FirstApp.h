//
// Created by auser on 3/21/25.
//
#ifndef FIRSTAPP_H
#define FIRSTAPP_H

#include "Window.h"
#include "Device.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"

#include <vector>
#include <memory>

class FirstApp {
public:
    FirstApp();

    void run();

    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    static constexpr std::string NAME = "VulkanApp";
private:

    void initVulkan();

    void mainLoop();

    void cleanup();

    Window window; /// glfw window
    Device device;
    std::unique_ptr<SwapChain> swapChain;
    std::unique_ptr<GraphicsPipeline> graphicsPipeline;
    RenderPass renderPass;

};


#endif //FIRSTAPP_H
