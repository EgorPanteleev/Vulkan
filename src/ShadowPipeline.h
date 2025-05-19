//
// Created by auser on 5/19/25.
//

#ifndef VULKAN_SHADOWPIPELINE_H
#define VULKAN_SHADOWPIPELINE_H

#include "GraphicsPipeline.h"
#include "ShadowDescriptorSet.h"

class ShadowPipeline {
public:
    ShadowPipeline(Context* context, ShadowDescriptorSet* descriptorSet, VkShaderModule& vertShaderModule);
    ~ShadowPipeline();

private:
    void createRenderPass();
    void createPipelineLayout(ShadowDescriptorSet* descriptorSet);
    void createGraphicsPipeline(VkShaderModule& vertShaderModule);
    void getPipelineConfigInfo( PipelineConfigInfo& configInfo );

    Context* mContext;
    VkRenderPass mRenderPass;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
};


#endif //VULKAN_SHADOWPIPELINE_H
