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
    DepthResources* depthResources;
    const UniformBuffers& uniformBuffers;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};

struct GraphicsPipelineRenderInfo {
    VkCommandBuffer commandBuffer;
    VkFramebuffer frameBuffer;
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    uint32_t indexCount;
    uint32_t currentFrame;
    VkExtent2D extent;
};

class GraphicsPipeline {
public:
    GraphicsPipeline(GraphicsPipelineCreateInfo& createInfo);
    ~GraphicsPipeline();
    /**
    * Getters
    */
    VkRenderPass renderPass() { return mRenderPass; }
    VkPipelineLayout pipelineLayout() { return mPipelineLayout; }
    VkPipeline graphicsPipeline() { return mGraphicsPipeline; }

    DescriptorSet* descriptorSet() { return mDescriptorSet; }

    void render(GraphicsPipelineRenderInfo& renderInfo);

private:
    void createDescriptorSet(GraphicsPipelineCreateInfo& createInfo);
    /**
     * Creating render pass
     */
    void createRenderPass();
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
    VkRenderPass mRenderPass;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
};


#endif //VULKAN_GRAPHICSPIPELINE_H
