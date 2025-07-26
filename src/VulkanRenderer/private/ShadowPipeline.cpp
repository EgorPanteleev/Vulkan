//
// Created by auser on 5/19/25.
//

#include "ShadowPipeline.hpp"
#include "Utils.hpp"

ShadowPipeline::ShadowPipeline(ShadowPipelineCreateInfo& createInfo):
                               mContext(createInfo.context), mShadowMap(new SampledImage(mContext)) {
    createShadowMap(createInfo);
    createDescriptorSet(createInfo);
    createPipelineLayout();
    createGraphicsPipeline(createInfo.vertShaderModule);
}
ShadowPipeline::~ShadowPipeline() {
    vkDestroyPipeline(mContext->device(), mGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(mContext->device(), mPipelineLayout, nullptr);
    delete mDescriptorSet;
    mShadowMap->destroy();
    delete mShadowMap;
}

void ShadowPipeline::createShadowMap(ShadowPipelineCreateInfo& createInfo) {
    ImageAllocateInfo allocateInfo{
        .format = Utils::findDepthFormat(mContext),
        .extent = createInfo.extent,
        .numSamples = VK_SAMPLE_COUNT_1_BIT,
        .imageUsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT,
    };
    mShadowMap->allocate(allocateInfo);
    ImageTransitInfo transitInfo{
        .src = VK_IMAGE_LAYOUT_UNDEFINED,
        .dst = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    mShadowMap->transit(transitInfo);
}

void ShadowPipeline::createDescriptorSet(ShadowPipelineCreateInfo& createInfo) {
    ShadowDescriptorSetCreateInfo shadowDescriptorSetCreateInfo{
        .context = createInfo.context,
        .uniformBuffers = createInfo.uniformBuffers
    };
    mDescriptorSet = new ShadowDescriptorSet(shadowDescriptorSetCreateInfo);
}

void ShadowPipeline::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &mDescriptorSet->descriptorSetLayout(),
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

    VkPipelineRenderingCreateInfo pipelineRenderingInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .colorAttachmentCount = 0,
            .pColorAttachmentFormats = nullptr,
            .depthAttachmentFormat = VK_FORMAT_D32_SFLOAT,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    Utils::PipelineConfigInfo configInfo;
    getPipelineConfigInfo( configInfo );
    VkGraphicsPipelineCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &pipelineRenderingInfo,
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
            .renderPass = nullptr,
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

    VkPipelineViewportStateCreateInfo viewportInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = nullptr, //dynamic
            .scissorCount = 1,
            .pScissors = nullptr //dynamic
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
    ImageTransitInfoCmd transitInfo{
        .commandBuffer = renderInfo.commandBuffer,
        .src = renderInfo.finalLayout,
        .dst = renderInfo.initialLayout,
    };
    mShadowMap->transit(transitInfo);
    VkClearValue clearValue{
            .depthStencil = {1.0f, 0}
    };

    VkRenderingAttachmentInfo depthAttachment = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = mShadowMap->imageView(),
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .clearValue = clearValue,
    };

    VkRenderingInfo renderingInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {
                    .offset = {0, 0},
                    .extent = mShadowMap->extent()
            },
            .layerCount = 1,
            .colorAttachmentCount = 0,
            .pColorAttachments = nullptr,
            .pDepthAttachment = &depthAttachment,
    };

    vkCmdBeginRendering(renderInfo.commandBuffer, &renderingInfo);

    vkCmdBindPipeline(renderInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

    // Set viewport and scissor for shadow pipeline
    VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(mShadowMap->extent().width),
            .height = static_cast<float>(mShadowMap->extent().height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };
    vkCmdSetViewport(renderInfo.commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{
        .offset = {0, 0},
        .extent = mShadowMap->extent()
    };
    vkCmdSetScissor(renderInfo.commandBuffer, 0, 1, &scissor);

    vkCmdBindDescriptorSets(renderInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout,
                            0, 1, &mDescriptorSet->descriptorSets()[renderInfo.currentFrame], 0, nullptr);

    VkBuffer vertexBuffers[] = {renderInfo.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(renderInfo.commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(renderInfo.commandBuffer, renderInfo.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(renderInfo.commandBuffer, renderInfo.indexCount, 1, 0, 0, 0);

    vkCmdEndRendering(renderInfo.commandBuffer);

    std::swap(transitInfo.src, transitInfo.dst);
    mShadowMap->transit(transitInfo);
}
