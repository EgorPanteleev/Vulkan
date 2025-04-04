//
// Created by auser on 4/2/25.
//

#ifndef VULKAN_UTILS_H
#define VULKAN_UTILS_H
#include <vulkan/vulkan.h>

//STL
#include <optional>
#include <vector>


#include "MessageLogger.h"

namespace Utils {

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

    VkDebugUtilsMessengerCreateInfoEXT createDebugMessengerCreateInfo();

    QueueFamilyIndices getQueueFamilies( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface );
    SwapChainSupportDetails getSwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    VkImageView createImageView(VkDevice device, VkImage image, VkImageViewType viewType, VkFormat format);
    uint32_t getImageCount(SwapChainSupportDetails swapChainSupport);
    std::vector<char> readFile(const std::string& filename);
    void createShaderModule( VkDevice device, const std::vector<char>& code, VkShaderModule* shaderModule );
    VkFramebuffer createFrameBuffer(VkDevice device, VkRenderPass renderPass, VkImageView imageView, VkExtent2D extent);
    VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance instance);
}

#endif //VULKAN_UTILS_H
