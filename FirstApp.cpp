//
// Created by auser on 3/21/25.
//

#include "FirstApp.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

FirstApp::FirstApp(): window( WIDTH, HEIGHT, NAME ),
                      device( window, MAX_FRAMES_IN_FLIGHT ),
                      syncObjects( device ),
                      swapChain( std::make_unique<SwapChain_old>(window, device, syncObjects ) ),
                      renderPass( device, swapChain->getImageFormat() ),
                      graphicsPipeline( std::make_unique<GraphicsPipeline>(
                              device, *swapChain, renderPass,
                              "/home/auser/dev/src/Vulkan/CompiledShaders/shader.vert.spv",
                              "/home/auser/dev/src/Vulkan/CompiledShaders/shader.frag.spv" ) ),
                      frameBuffers( device, *swapChain, renderPass ),
                      commandBuffers( device, *swapChain, renderPass, frameBuffers,
                                     *graphicsPipeline, syncObjects ) {
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
        drawFrame();
    }
    vkDeviceWaitIdle( device.getLogicalDevice() );
}

void FirstApp::cleanup() {
}

void FirstApp::drawFrame() {
    uint32_t imageIndex;
    swapChain->acquireNextImage( &imageIndex );
    commandBuffers.recordCommandBuffer( imageIndex );
    commandBuffers.submitCommandBuffer( &imageIndex );
    swapChain->updateCurrentFrame();
}
