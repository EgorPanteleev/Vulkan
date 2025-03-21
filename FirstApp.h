//
// Created by auser on 3/21/25.
//
#ifndef FIRSTAPP_H
#define FIRSTAPP_H

#include "Window.h"
#include "Instance.h"

#include <vector>

class FirstApp {
public:
    FirstApp();

    void run();

    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    static constexpr std::string NAME = "VulkanApp";
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
private:

    void initVulkan();

    void mainLoop();

    void cleanup();

    Window window; // glfw window
    Instance instance; // instance is a connection between app and vulkan

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
};


#endif //FIRSTAPP_H
