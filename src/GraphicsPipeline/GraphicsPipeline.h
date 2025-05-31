//
// Created by auser on 3/22/25.
//

#ifndef VULKAN_GRAPHICSPIPELINE_H
#define VULKAN_GRAPHICSPIPELINE_H

#include "Context.h"
#include "SwapChain.h"
#include "DescriptorSet.h"
#include "Utils.h"

class GraphicsPipeline {
public:
    GraphicsPipeline(Context* context, SwapChain* swapChain, DescriptorSet* descriptorSet,
                     VkShaderModule& mVertShaderModule, VkShaderModule& mFragShaderModule);
    ~GraphicsPipeline();
    /**
    * Getters
    */
    VkRenderPass renderPass() { return mRenderPass; }
    VkPipelineLayout pipelineLayout() { return mPipelineLayout; }
    VkPipeline graphicsPipeline() { return mGraphicsPipeline; }

private:
    /**
     * Creating render pass
     */
    void createRenderPass();
    /**
     * Creating pipeline layout
     */
    void createPipelineLayout(DescriptorSet* descriptorSet);
    /**
     * Creating graphics pipeline
     */
    void createGraphicsPipeline( VkShaderModule& vertShaderModule, VkShaderModule& fragShaderModule );
    void getPipelineConfigInfo( Utils::PipelineConfigInfo& configInfo );

    Context* mContext;
    SwapChain* mSwapChain;
    VkRenderPass mRenderPass;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
};


#endif //VULKAN_GRAPHICSPIPELINE_H
