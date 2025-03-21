//
// Created by auser on 3/21/25.
//

#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    bool isComplete() { return graphicsFamily.has_value(); }
};

class Device {
public:
    Device();
    ~Device();

private:
    void createInstance();

    void pickPhysicalDevice();

    void createLogicalDevice();

    bool isDeviceSuitable( VkPhysicalDevice device );

    QueueFamilyIndices findQueueFamilies( VkPhysicalDevice device );

    std::vector<const char *> getRequiredExtensions();

    void hasGflwRequiredInstanceExtensions();

    bool checkValidationLayerSupport();

    void setupDebugMessenger();

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    VkInstance instance; /// Instance is a connection between app and vulkan
    VkDebugUtilsMessengerEXT debugMessenger; /// Using for catch diff errors and leaks in debug
    VkPhysicalDevice physicalDevice; /// Physical device
    VkDevice device; /// logical device
};


#endif //VULKAN_DEVICE_H
