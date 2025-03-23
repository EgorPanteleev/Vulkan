//
// Created by auser on 3/21/25.
//

#include "FirstApp.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

FirstApp::FirstApp(): window( WIDTH, HEIGHT, NAME ),
                      device( window ),
                      swapChain( std::make_unique<SwapChain>( window, device ) ),
                      renderPass( device, swapChain->getImageFormat() ),
                      graphicsPipeline( std::make_unique<GraphicsPipeline>(
                              device, *swapChain, renderPass,
                              "/home/auser/dev/src/Vulkan/CompiledShaders/shader.vert.spv",
                              "/home/auser/dev/src/Vulkan/CompiledShaders/shader.frag.spv" ) ),
                      frameBuffers( device, *swapChain, renderPass ),
                      commandBuffer( device, *swapChain, renderPass, frameBuffers, *graphicsPipeline ) {

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
