//
// Created by auser on 3/21/25.
//

#include "FirstApp.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

FirstApp::FirstApp(): window( WIDTH, HEIGHT, NAME ), device() {
}

void FirstApp::run() {
    initVulkan();
    mainLoop();
    cleanup();
}

void FirstApp::initVulkan() {
}

void FirstApp::mainLoop() {
    while ( !window.shouldClose() ) {
        glfwPollEvents();
    }
}

void FirstApp::cleanup() {
}