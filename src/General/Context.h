//
// Created by auser on 4/2/25.
//

#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include <vk_mem_alloc.h>
#include <vector>

#include "Window.h"

struct ContextCreateInfo {
    WindowCreateInfo windowCreateInfo{};
    uint32_t maxFramesInFlight = 2;
    std::vector<const char*> validationLayers;
    std::vector<const char*> deviceExtensions;
    bool enableValidationLayers = false;
};

class Context {
public:
    Context(const ContextCreateInfo& createInfo);
    ~Context();

    /// Getters
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
    /// Creating instance
    void createInstance();
    bool checkValidationLayerSupport();
    std::vector<const char *> getRequiredExtensions();
    void checkGflwRequiredInstanceExtensions();

    /// Picking physical device
    void pickPhysicalDevice();
    bool isDeviceSuitable( VkPhysicalDevice device );
    bool checkDeviceExtensionSupport( VkPhysicalDevice device );

    /// Creating logical device
    void createLogicalDevice();

    /// Other
    void createAllocator();
    VkQueue getQueue(uint32_t index) const;

    //    std::vector<const char*> mValidationLayers = { "VK_LAYER_KHRONOS_validation" };
    //    std::vector<const char *> mDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    //                                                         VK_KHR_MAINTENANCE_1_EXTENSION_NAME};

    uint32_t mMaxFramesInFlight;
    std::vector<const char*> mValidationLayers;
    std::vector<const char *> mDeviceExtensions;
    bool mEnableValidationLayers;

//#ifdef NDEBUG
//    const bool mEnableValidationLayers = false;
//#else
//    const bool mEnableValidationLayers = true;
//#endif
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
