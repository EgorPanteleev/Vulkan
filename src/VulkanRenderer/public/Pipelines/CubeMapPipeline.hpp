//
// Created by igor on 8/25/25.
//

#ifndef VULKAN_CUBEMAPPIPELINE_HPP
#define VULKAN_CUBEMAPPIPELINE_HPP

#include "Context.hpp"
#include "SwapChain.hpp"
#include "CubeMapDescriptorSet.hpp"
#include "Utils.hpp"

//struct GraphicsPipelineCreateInfo {
//    Context* context;
//    SwapChain* swapChain;
//    VulkanModelLoader* loader;
//    SampledImage* shadowMap;
//    const UniformBuffers& uniformBuffers;
//    VkShaderModule vertShaderModule;
//    VkShaderModule fragShaderModule;
//    bool enableMSAA = false;
//};
//
//struct LayoutAttachment {
//    VkImageLayout initialLayout;
//    VkImageLayout finalLayout;
//};
//
//struct GraphicsPipelineRenderInfo {
//    VkCommandBuffer commandBuffer;
//    VkBuffer vertexBuffer;
//    VkBuffer indexBuffer;
//    uint32_t indexCount;
//    uint32_t currentFrame;
//    VkExtent2D extent;
//    Image* presentImage;
//    LayoutAttachment colorLayoutAttachment;
//    LayoutAttachment depthLayoutAttachment;
//};

struct CubeMapPipelineCreateInfo {
    Context* context;
    SwapChain* swapChain;
    VulkanModelLoader* loader;
    const UniformBuffers& uniformBuffers;
    std::string vertShaderPath;
    std::string fragShaderPath;
};

struct CubeMapPipelineRenderInfo {
    VkCommandBuffer commandBuffer;
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    uint32_t indexCount;
    uint32_t currentFrame;
    VkExtent2D extent;
    Image* presentImage;
    VkImageLayout initialLayout;
    VkImageLayout finalLayout;
};

class CubeMapPipeline {
public:
    CubeMapPipeline(CubeMapPipelineCreateInfo& createInfo);
    ~CubeMapPipeline();
    /**
    * Getters
    */
    VkPipelineLayout pipelineLayout() { return mPipelineLayout; }
    VkPipeline pipeline() { return mPipeline; }

    CubeMapDescriptorSet* descriptorSet() { return mDescriptorSet; }

    void render(CubeMapPipelineRenderInfo& renderInfo);

    void updateDescriptorSet() { mDescriptorSet->updateDescriptorSets(); }

private:
    void createDescriptorSet(CubeMapPipelineCreateInfo& createInfo);
    /**
     * Creating pipeline layout
     */
    void createPipelineLayout();
    /**
     * Creating graphics pipeline
     */
    void createGraphicsPipeline(const std::string& vertPath, const std::string& fragPath);
    void getPipelineConfigInfo(Utils::PipelineConfigInfo& configInfo);

    Context* mContext;
    SwapChain* mSwapChain;
    CubeMapDescriptorSet* mDescriptorSet;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mPipeline;
    VkShaderModule mVertShader;
    VkShaderModule mFragShader;
};

#endif //VULKAN_CUBEMAPPIPELINE_HPP
