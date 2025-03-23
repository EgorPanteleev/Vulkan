//
// Created by auser on 3/22/25.
//

#ifndef VULKAN_GRAPHICSPIPELINE_H
#define VULKAN_GRAPHICSPIPELINE_H

#include "Device.h"
#include "SwapChain.h"
#include "RenderPass.h"

#include <string>

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
    GraphicsPipeline( Device& device, SwapChain& swapChain, RenderPass& renderPass,
                      const std::string& vertShaderPath, const std::string& fragShaderPath );
    ~GraphicsPipeline();

    VkPipeline operator()() { return graphicsPipeline; }
private:

    void createGraphicsPipeline( const std::string& vertShaderPath, const std::string& fragShaderPath );

    void createPipelineLayout();

    void createShaderModule( const std::vector<char>& code, VkShaderModule* shaderModule );

    void getPipelineConfigInfo( PipelineConfigInfo& configInfo );

    Device& device;
    SwapChain& swapChain;
    RenderPass& renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};


#endif //VULKAN_GRAPHICSPIPELINE_H
