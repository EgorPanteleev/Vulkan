//
// Created by auser on 3/21/25.
//

#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>

#include "Window.h"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class Device {
public:
    Device( Window& window );
    ~Device();

    SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport( physicalDevice ); }

    VkSurfaceKHR getSurface() { return surface; }

    QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies( physicalDevice ); }

    VkDevice getLogicalDevice() { return logicalDevice; }

    VkCommandPool getCommandPool() { return commandPool; }

    VkQueue getGraphicsQueue() { return graphicsQueue; }

    VkQueue getPresentQueue() { return presentQueue; }

private:
    void createInstance();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void createSurface();

    bool isDeviceSuitable( VkPhysicalDevice device );

    bool checkDeviceExtensionSupport( VkPhysicalDevice device );

    QueueFamilyIndices findQueueFamilies( VkPhysicalDevice device );

    std::vector<const char *> getRequiredExtensions();

    void hasGflwRequiredInstanceExtensions();

    bool checkValidationLayerSupport();

    void setupDebugMessenger();

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    void createCommandPool();

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    Window& window;
    VkInstance instance; /// Instance is a connection between app and vulkan
    VkDebugUtilsMessengerEXT debugMessenger; /// Usinug for catch diff errors and leaks in debug
    VkPhysicalDevice physicalDevice; /// Physical device
    VkDevice logicalDevice; /// logical device
    VkSurfaceKHR surface; /// connection between vulkan and os window
    VkQueue graphicsQueue; /// graphics queue
    VkQueue presentQueue; /// present queue
    VkCommandPool commandPool;
};


#endif //VULKAN_DEVICE_H
