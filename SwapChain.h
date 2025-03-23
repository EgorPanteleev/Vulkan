//
// Created by auser on 3/22/25.
//

#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>

#include "Window.h"
#include "Device.h"
#include "SyncObjects.h"

class SwapChain {
public:
    SwapChain( Window& window, Device& device, SyncObjects& syncObjects );

    ~SwapChain();

    VkSwapchainKHR operator()() { return swapChain; }

    VkFormat getImageFormat() { return swapChainImageFormat; }

    VkExtent2D getExtent() { return swapChainExtent; }

    std::vector<VkImageView>& getImageViews() { return swapChainImageViews; }

    VkResult acquireNextImage( uint32_t *imageIndex );

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
};


#endif //VULKAN_SWAPCHAIN_H
