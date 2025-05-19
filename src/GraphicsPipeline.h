//
// Created by auser on 3/22/25.
//

#ifndef VULKAN_GRAPHICSPIPELINE_H
#define VULKAN_GRAPHICSPIPELINE_H

#include "Context.h"
#include "SwapChain.h"
#include "DescriptorSet.h"

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
    void getPipelineConfigInfo( PipelineConfigInfo& configInfo );

    Context* mContext;
    SwapChain* mSwapChain;
    VkRenderPass mRenderPass;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
};


#endif //VULKAN_GRAPHICSPIPELINE_H
