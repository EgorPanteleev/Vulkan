//
// Created by auser on 3/22/25.
//

#ifndef VULKAN_SWAPCHAIN_OLD_H
#define VULKAN_SWAPCHAIN_OLD_H

#include <vulkan/vulkan.h>
#include <vector>

#include "Window.h"
#include "Device.h"
#include "SyncObjects.h"

class SwapChain_old {
public:
    SwapChain_old(Window& window, Device& device, SyncObjects& syncObjects );

    ~SwapChain_old();

    VkSwapchainKHR operator()() { return swapChain; }

    VkFormat getImageFormat() { return swapChainImageFormat; }

    VkExtent2D getExtent() { return swapChainExtent; }

    std::vector<VkImageView>& getImageViews() { return swapChainImageViews; }

    VkResult acquireNextImage( uint32_t *imageIndex );

    uint32_t getCurrentFrame() const { return currentFrame; }

    void setCurrentFrame( uint32_t currFrame ) { currentFrame = currFrame; }

    void updateCurrentFrame();

private:

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void createSwapChain();

    void createImageViews();

    Window& window;
    Device& device;
    SyncObjects& syncObjects;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    uint32_t currentFrame = 0;
};


#endif //VULKAN_SWAPCHAIN_OLD_H
