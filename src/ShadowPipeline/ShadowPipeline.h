//
// Created by auser on 5/19/25.
//

#ifndef VULKAN_SHADOWPIPELINE_H
#define VULKAN_SHADOWPIPELINE_H

#include "ShadowDescriptorSet.h"
#include "Utils.h"

class ShadowPipeline {
public:
    ShadowPipeline(Context* context, ShadowDescriptorSet* descriptorSet, VkShaderModule& vertShaderModule);
    ~ShadowPipeline();

    VkRenderPass renderPass() { return mRenderPass; }
    VkPipelineLayout pipelineLayout() { return mPipelineLayout; }
    VkPipeline graphicsPipeline() { return mGraphicsPipeline; }

private:
    void createRenderPass();
    void createPipelineLayout(ShadowDescriptorSet* descriptorSet);
    void createGraphicsPipeline(VkShaderModule& vertShaderModule);
    void getPipelineConfigInfo( Utils::PipelineConfigInfo& configInfo );

    Context* mContext;
    VkRenderPass mRenderPass;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
};


#endif //VULKAN_SHADOWPIPELINE_H
