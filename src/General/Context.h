//
// Created by auser on 4/2/25.
//

#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vk_mem_alloc.h"
//STL
#include <vector>
#include <memory>

#include "Window.h"

class Context {
public:
    Context();
    ~Context();

    /**
     *  Getters
     */
    VkInstance instance() { return mInstance; }
    Window& window() { return mWindow; }
    GLFWwindow* glfwWindow() { return mWindow.window(); }
    VkSurfaceKHR surface() { return mSurface; }
    VkPhysicalDevice physicalDevice() { return mPhysicalDevice; }
    VkDevice device() { return mDevice; }
    VmaAllocator allocator() { return mAllocator; }
    uint32_t maxFramesInFlight() { return mMaxFramesInFlight; }
    VkQueue graphicsQueue() { return mGraphicsQueue; }
    VkQueue presentQueue() { return mPresentQueue; }

private:
    /**
     *  Creating instance
     */
    void createInstance();
    bool checkValidationLayerSupport();
    std::vector<const char *> getRequiredExtensions();
    void checkGflwRequiredInstanceExtensions();
    /**
     * Picking physical device
     */
    void pickPhysicalDevice();
    bool isDeviceSuitable( VkPhysicalDevice device );
    bool checkDeviceExtensionSupport( VkPhysicalDevice device );
    /**
     * Creating logical device
     */
    void createLogicalDevice();

    void createAllocator();
    VkQueue getQueue(uint32_t index) const;
    const uint32_t mMaxFramesInFlight = 2;
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME,VK_KHR_MAINTENANCE_1_EXTENSION_NAME};
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
    VkInstance mInstance;
    Window mWindow;
    VkSurfaceKHR mSurface;
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;
    VmaAllocator mAllocator;
    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    VkDebugUtilsMessengerEXT mDebugMessenger;
};


#endif //VULKAN_CONTEXT_H
