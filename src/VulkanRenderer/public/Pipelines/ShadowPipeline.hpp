//
// Created by auser on 5/19/25.
//

#ifndef VULKAN_SHADOWPIPELINE_H
#define VULKAN_SHADOWPIPELINE_H

#include "ShadowDescriptorSet.hpp"
#include "Utils.hpp"
#include "SampledImage.hpp"

struct ShadowPipelineCreateInfo {
    Context* context;
    const UniformBuffers& uniformBuffers;
    std::string vertShaderPath;
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
    VkPipeline graphicsPipeline() { return mPipeline; }
    ShadowDescriptorSet* descriptorSet() { return mDescriptorSet; }
    SampledImage* shadowMap() { return mShadowMap; }

    void render(ShadowPipelineRenderInfo& renderInfo);

private:
    void createShadowMap(ShadowPipelineCreateInfo& createInfo);
    void createDescriptorSet(ShadowPipelineCreateInfo& createInfo);
    void createPipelineLayout();
    void createGraphicsPipeline(const std::string& vertShaderPath);
    void getPipelineConfigInfo(Utils::PipelineConfigInfo& configInfo);

    Context* mContext;
    ShadowDescriptorSet* mDescriptorSet;
    SampledImage* mShadowMap;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mPipeline;
    VkShaderModule mVertShader;
};

#endif //VULKAN_SHADOWPIPELINE_H
