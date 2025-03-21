//
// Created by auser on 3/21/25.
//
#ifndef FIRSTAPP_H
#define FIRSTAPP_H

#include "Window.h"
#include "Device.h"

#include <vector>

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
};


#endif //FIRSTAPP_H
