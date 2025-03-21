//
// Created by auser on 3/21/25.
//

#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class Instance {
public:
    Instance();
    ~Instance();

private:
    void createInstance();

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

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
};


#endif //VULKAN_INSTANCE_H
