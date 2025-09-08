//
// Created by auser on 4/2/25.
//

#ifndef VULKAN_UTILS_H
#define VULKAN_UTILS_H
#include "Vertex.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

//STL
#include <optional>
#include <vector>
#include "Message.hpp"

class Context;

#define VK_CHECK(res, msg)                                                \
    do {                                                                  \
        VkResult err = res;                                               \
        if (err != VK_SUCCESS) {                                          \
            throw std::runtime_error(msg);                                \
        }                                                                 \
    } while (0)

namespace Utils {

    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo;
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

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
    uint32_t getImageCount(const SwapChainSupportDetails& swapChainSupport);
    std::vector<char> readFile(const std::string& filename);
    void createShaderModule(VkDevice device, const std::vector<char>& code, VkShaderModule* shaderModule);
    void loadShader(VkDevice device, const std::string& shaderPath, VkShaderModule& module);
    VkFramebuffer createFrameBuffer(VkDevice device, VkRenderPass renderPass,
                                    std::vector<VkImageView> attachments, VkExtent2D extent);
    VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance instance);

    VkCommandPool createCommandPool(Context* context,VkCommandPoolCreateFlags flags);

    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VmaAllocator allocator, VmaAllocation& allocation, VmaMemoryUsage allocUsage,
                      VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer);
    void copyDataToBuffer(VmaAllocator allocator, VmaAllocation& allocation, void* data, VkDeviceSize bufferSize );
    void copyBuffer(Context* context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    VkCommandBuffer createCommandBuffer(VkDevice device, VkCommandPool commandPool);

    VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);

    void endSingleTimeCommands(Context* context, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
    bool hasStencilComponent(VkFormat format);
    void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, uint32_t mipLevels,
                               VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
                               uint32_t level = 0, uint32_t levelCount = 0, uint32_t layer = 0, uint32_t layerCount = 1);
    void transitionImageLayout(Context* context, VkImage image, uint32_t mipLevels, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout,
                               uint32_t level = 0, uint32_t levelCount = 0, uint32_t layer = 0, uint32_t layerCount = 1);
    void copyBufferToImage(Context* context, VkBuffer buffer, VkImage image,
                           uint32_t width, uint32_t height, int mipLevel = 0, int layer = 0);

    VkFormat findSupportedFormat(Context* context, const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

    VkFormat findDepthFormat(Context* context);

    VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice);
}

#endif //VULKAN_UTILS_H
