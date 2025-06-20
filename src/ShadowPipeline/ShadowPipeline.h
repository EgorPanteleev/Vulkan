//
// Created by auser on 5/19/25.
//

#ifndef VULKAN_SHADOWPIPELINE_H
#define VULKAN_SHADOWPIPELINE_H

#include "ShadowDescriptorSet.h"
#include "Utils.h"

struct ShadowPipelineCreateInfo {
    Context* context;
    const UniformBuffers& uniformBuffers;
    VkShaderModule& vertShaderModule;
    VkExtent2D extent;
};

struct ShadowPipelineRenderInfo {
    VkCommandBuffer commandBuffer;
    VkFramebuffer frameBuffer;
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    uint32_t indexCount;
    uint32_t currentFrame;
};

class ShadowPipeline {
public:
    ShadowPipeline(ShadowPipelineCreateInfo& createInfo);
    ~ShadowPipeline();

    VkRenderPass renderPass() { return mRenderPass; }
    VkPipelineLayout pipelineLayout() { return mPipelineLayout; }
    VkPipeline graphicsPipeline() { return mGraphicsPipeline; }
    ShadowDescriptorSet* descriptorSet() { return mDescriptorSet; }

    void render(ShadowPipelineRenderInfo& renderInfo);

private:
    void createDescriptorSet(ShadowPipelineCreateInfo& createInfo);
    void createRenderPass();
    void createPipelineLayout();
    void createGraphicsPipeline(VkShaderModule& vertShaderModule);
    void getPipelineConfigInfo(Utils::PipelineConfigInfo& configInfo);

    Context* mContext;
    ShadowDescriptorSet* mDescriptorSet;
    VkRenderPass mRenderPass;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
    VkExtent2D mShadowMapExtent;
};


#endif //VULKAN_SHADOWPIPELINE_H
