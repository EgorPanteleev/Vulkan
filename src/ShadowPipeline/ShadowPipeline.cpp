//
// Created by auser on 5/19/25.
//

#include "ShadowPipeline.h"
#include "Utils.h"

ShadowPipeline::ShadowPipeline(Context* context, ShadowDescriptorSet* descriptorSet, VkShaderModule& vertShaderModule):
                               mContext(context), mDescriptorSet(descriptorSet) {
    createRenderPass();
    createPipelineLayout(descriptorSet);
    createGraphicsPipeline(vertShaderModule);
}
ShadowPipeline::~ShadowPipeline() {
    vkDestroyPipeline(mContext->device(), mGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(mContext->device(), mPipelineLayout, nullptr);
    vkDestroyRenderPass(mContext->device(), mRenderPass, nullptr);
}

void ShadowPipeline::createRenderPass() {
    VkAttachmentDescription depthAttachment{
            .format = Utils::findDepthFormat(mContext),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    VkAttachmentReference depthAttachmentRef{
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL
    };

    VkSubpassDependency dependency{
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
    };

    VkSubpassDescription subpass{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 0,
            .pDepthStencilAttachment = &depthAttachmentRef
    };

    std::vector<VkAttachmentDescription> attachments = {depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = (uint32_t)attachments.size(),
            .pAttachments = attachments.data(),
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency
    };

    if (vkCreateRenderPass(mContext->device(), &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
    INFO << "Created render pass";
}

void ShadowPipeline::createPipelineLayout(ShadowDescriptorSet* descriptorSet) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &descriptorSet->descriptorSetLayout(),
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
    };

    if (vkCreatePipelineLayout(mContext->device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }
    INFO << "Created pipeline layout!";
}

void ShadowPipeline::createGraphicsPipeline(VkShaderModule& vertShaderModule) {
    VkPipelineShaderStageCreateInfo shaderStages[1] = {
            {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = vertShaderModule,
                    .pName = "main",
                    .pSpecializationInfo = nullptr
            }
    };

    VkPipelineDepthStencilStateCreateInfo depthStencil{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f,
    };

    Utils::PipelineConfigInfo configInfo;
    getPipelineConfigInfo( configInfo );
    VkGraphicsPipelineCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 1,
            .pStages = shaderStages,
            .pVertexInputState = &configInfo.vertexInputInfo,
            .pInputAssemblyState = &configInfo.inputAssemblyInfo,
            .pViewportState = &configInfo.viewportInfo,
            .pRasterizationState = &configInfo.rasterizationInfo,
            .pMultisampleState = &configInfo.multisampleInfo,
            .pDepthStencilState = &depthStencil,
            .pColorBlendState = &configInfo.colorBlendInfo,
            .pDynamicState = &configInfo.dynamicStateInfo,
            .layout = mPipelineLayout,
            .renderPass = mRenderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
    };

    if (vkCreateGraphicsPipelines(mContext->device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }
    INFO << "Created graphics pipeline!";
}

void ShadowPipeline::getPipelineConfigInfo( Utils::PipelineConfigInfo& configInfo ) {
    static auto bindingDescription = Vertex::getBindingDescription();
    static auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size(),
            .pVertexAttributeDescriptions = attributeDescriptions.data()
    };
    configInfo.vertexInputInfo = vertexInputInfo;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
    };
    configInfo.inputAssemblyInfo = inputAssemblyInfo;

    VkExtent2D shadowMapExtent = {1024, 1024};
    VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = (float)shadowMapExtent.width,
            .height = (float)shadowMapExtent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
    };

    VkRect2D scissor{
            .offset = {0, 0},
            .extent = shadowMapExtent
    };

    VkPipelineViewportStateCreateInfo viewportInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor
    };
    configInfo.viewportInfo = viewportInfo;

    VkPipelineRasterizationStateCreateInfo rasterizationInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_TRUE,
            .depthBiasConstantFactor = 4.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 1.5f,
            .lineWidth = 1.0f
    };
    configInfo.rasterizationInfo = rasterizationInfo;

    VkPipelineMultisampleStateCreateInfo multisampleInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE
    };
    configInfo.multisampleInfo = multisampleInfo;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{
            .blendEnable = VK_FALSE,
            .colorWriteMask = 0
    };
    configInfo.colorBlendAttachment = colorBlendAttachment;

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .attachmentCount = 0,
            .pAttachments = nullptr
    };
    configInfo.colorBlendInfo = colorBlendInfo;

    configInfo.dynamicStateEnables = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size()),
            .pDynamicStates = configInfo.dynamicStateEnables.data()
    };
    configInfo.dynamicStateInfo = dynamicStateInfo;
}


void ShadowPipeline::render(ShadowPipelineRenderInfo& renderInfo) {
    VkClearValue shadowClearValue{
            .depthStencil = {1.0f, 0}
    };
    VkRenderPassBeginInfo shadowPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = mRenderPass,
            .framebuffer = renderInfo.frameBuffer,
            .renderArea = {
                    .offset = {0, 0},
                    .extent = {1024, 1024}
            },
            .clearValueCount = 1,
            .pClearValues = &shadowClearValue
    };

    vkCmdBeginRenderPass(renderInfo.commandBuffer, &shadowPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(renderInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

    // Set viewport and scissor for shadow pipeline
    VkViewport shadowViewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(1024),
            .height = static_cast<float>(1024),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };
    vkCmdSetViewport(renderInfo.commandBuffer, 0, 1, &shadowViewport);

    VkRect2D shadowScissor{
        .offset = {0, 0},
        .extent = {1024, 1024}
    };
    vkCmdSetScissor(renderInfo.commandBuffer, 0, 1, &shadowScissor);

    VkBuffer vertexBuffers[] = {renderInfo.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(renderInfo.commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(renderInfo.commandBuffer, renderInfo.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(renderInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout,
                            0, 1, &mDescriptorSet->descriptorSets()[renderInfo.currentFrame], 0, nullptr);

    vkCmdDrawIndexed(renderInfo.commandBuffer, renderInfo.indexCount, 1, 0, 0, 0);

    vkCmdEndRenderPass(renderInfo.commandBuffer);
}
