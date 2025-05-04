//
// Created by auser on 4/2/25.
//

#ifndef VULKAN_UTILS_H
#define VULKAN_UTILS_H
#include "Vertex.h"

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

//STL
#include <optional>
#include <vector>
#include "MessageLogger.h"

class Context;

struct Model {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

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

    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    SwapChainSupportDetails getSwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    VkImageView createImageView(VkDevice device, VkImage image, VkImageViewType viewType,
                                VkFormat format, VkImageAspectFlags aspectFlags);
    uint32_t getImageCount(const SwapChainSupportDetails& swapChainSupport);
    std::vector<char> readFile(const std::string& filename);
    void createShaderModule( VkDevice device, const std::vector<char>& code, VkShaderModule* shaderModule );
    VkFramebuffer createFrameBuffer(VkDevice device, VkRenderPass renderPass,
                                    VkImageView imageView, VkImageView depthImageView, VkExtent2D extent);
    VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance instance);

    VkCommandPool createCommandPool(Context* context,VkCommandPoolCreateFlags flags);

    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VmaAllocator allocator, VmaAllocation& allocation, VmaMemoryUsage allocUsage,
                      VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer);
    void copyDataToBuffer(VmaAllocator allocator, VmaAllocation& allocation, void* data, VkDeviceSize bufferSize );
    void copyBuffer(Context* context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void createImage(VmaAllocator allocator, VmaAllocation& imageAllocation, VmaMemoryUsage allocUsage,
                     VkImage& image, uint32_t width, uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags imageUsage);
    VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);

    void endSingleTimeCommands(Context* context, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
    bool hasStencilComponent(VkFormat format);
    void transitionImageLayout(Context* context, VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(Context* context, VkBuffer buffer, VkImage image,
                           uint32_t width, uint32_t height );

    VkFormat findSupportedFormat(Context* context, const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

    VkFormat findDepthFormat(Context* context);

    void loadModel(Model& model, const std::string& path);
}

#endif //VULKAN_UTILS_H
