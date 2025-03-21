//
// Created by auser on 3/21/25.
//

#include "FirstApp.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

FirstApp::FirstApp(): window( WIDTH, HEIGHT, NAME ) {
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