//
// Created by auser on 3/22/25.
//

#ifndef VULKAN_GRAPHICSPIPELINE_H
#define VULKAN_GRAPHICSPIPELINE_H

#include "Context.h"
#include "SwapChain.h"
#include "DescriptorSet.h"
#include "Utils.h"


struct GraphicsPipelineCreateInfo {
    Context* context;
    SwapChain* swapChain;
    VulkanModelLoader* loader;
    SampledImage* shadowMap;
    const UniformBuffers& uniformBuffers;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};

struct LayoutAttachment {
    VkImageLayout initialLayout;
    VkImageLayout finalLayout;
};

struct GraphicsPipelineRenderInfo {
    VkCommandBuffer commandBuffer;
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    uint32_t indexCount;
    uint32_t currentFrame;
    VkExtent2D extent;
    Image* presentImage;
    LayoutAttachment colorLayoutAttachment;
    LayoutAttachment depthLayoutAttachment;
};

class GraphicsPipeline {
public:
    GraphicsPipeline(GraphicsPipelineCreateInfo& createInfo);
    ~GraphicsPipeline();
    /**
    * Getters
    */
    VkPipelineLayout pipelineLayout() { return mPipelineLayout; }
    VkPipeline graphicsPipeline() { return mGraphicsPipeline; }

    DescriptorSet* descriptorSet() { return mDescriptorSet; }

    Image* colorBuffer() { return mColorBuffer; }
    Image* depthBuffer() { return mDepthBuffer; }

    void render(GraphicsPipelineRenderInfo& renderInfo);

    void updateDescriptorSet() { mDescriptorSet->updateDescriptorSets(); }

    void recreateBuffers();

private:
    void createColorBuffer();
    void createDepthBuffer();

    void createDescriptorSet(GraphicsPipelineCreateInfo& createInfo);
    /**
     * Creating pipeline layout
     */
    void createPipelineLayout();
    /**
     * Creating graphics pipeline
     */
    void createGraphicsPipeline( VkShaderModule& vertShaderModule, VkShaderModule& fragShaderModule );
    void getPipelineConfigInfo( Utils::PipelineConfigInfo& configInfo );

    Context* mContext;
    SwapChain* mSwapChain;
    DescriptorSet* mDescriptorSet;
    Image* mColorBuffer;
    Image* mDepthBuffer;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
    VkPipelineCache mPipelineCache;
};


#endif //VULKAN_GRAPHICSPIPELINE_H
