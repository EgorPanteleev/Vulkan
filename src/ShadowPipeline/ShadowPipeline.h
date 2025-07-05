//
// Created by auser on 5/19/25.
//

#ifndef VULKAN_SHADOWPIPELINE_H
#define VULKAN_SHADOWPIPELINE_H

#include "ShadowDescriptorSet.h"
#include "Utils.h"
#include "SampledImage.h"

struct ShadowPipelineCreateInfo {
    Context* context;
    const UniformBuffers& uniformBuffers;
    VkShaderModule& vertShaderModule;
    VkExtent2D extent;
};

struct ShadowPipelineRenderInfo {
    VkCommandBuffer commandBuffer;
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    uint32_t indexCount;
    uint32_t currentFrame;
    VkImageLayout initialLayout;
    VkImageLayout finalLayout;
};

class ShadowPipeline {
public:
    ShadowPipeline(ShadowPipelineCreateInfo& createInfo);
    ~ShadowPipeline();

    VkPipelineLayout pipelineLayout() { return mPipelineLayout; }
    VkPipeline graphicsPipeline() { return mGraphicsPipeline; }
    ShadowDescriptorSet* descriptorSet() { return mDescriptorSet; }
    SampledImage* shadowMap() { return mShadowMap; }

    void render(ShadowPipelineRenderInfo& renderInfo);

private:
    void createShadowMap(ShadowPipelineCreateInfo& createInfo);
    void createDescriptorSet(ShadowPipelineCreateInfo& createInfo);
    void createPipelineLayout();
    void createGraphicsPipeline(VkShaderModule& vertShaderModule);
    void getPipelineConfigInfo(Utils::PipelineConfigInfo& configInfo);

    Context* mContext;
    ShadowDescriptorSet* mDescriptorSet;
    SampledImage* mShadowMap;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
};

#endif //VULKAN_SHADOWPIPELINE_H
