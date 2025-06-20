//
// Created by auser on 3/22/25.
//

#include "GraphicsPipeline.h"
#include "Utils.h"


GraphicsPipeline::GraphicsPipeline(GraphicsPipelineCreateInfo& createInfo):
                                   mContext(createInfo.context), mSwapChain(createInfo.swapChain) {
    createDescriptorSet(createInfo);
    createRenderPass();
    createPipelineLayout();
    createGraphicsPipeline( createInfo.vertShaderModule, createInfo.fragShaderModule );
}

GraphicsPipeline::~GraphicsPipeline(){
    delete mDescriptorSet;
    vkDestroyPipeline(mContext->device(), mGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(mContext->device(), mPipelineLayout, nullptr);
    vkDestroyRenderPass(mContext->device(), mRenderPass, nullptr);
}

void GraphicsPipeline::createDescriptorSet(GraphicsPipelineCreateInfo& createInfo) {
    DescriptorSetCreateInfo descriptorSetCreateInfo{
            .context = createInfo.context,
            .loader = createInfo.loader,
            .depthResources = createInfo.depthResources,
            .uniformBuffers = createInfo.uniformBuffers
    };
    mDescriptorSet = new DescriptorSet(descriptorSetCreateInfo);
}

void GraphicsPipeline::createRenderPass() {
    auto msaaSamples = Utils::getMaxUsableSampleCount(mContext->physicalDevice());
    VkAttachmentDescription colorAttachment {
            .format = mSwapChain->format(),
            .samples = msaaSamples,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference colorAttachmentRef{
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription depthAttachment{
            .format = Utils::findDepthFormat(mContext),
            .samples = msaaSamples,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference depthAttachmentRef{
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription colorAttachmentResolve{
            .format = mSwapChain->format(),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorAttachmentResolveRef{
            .attachment = 2,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDependency dependency{
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    };

    VkSubpassDescription subpass{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentRef,
            .pResolveAttachments = &colorAttachmentResolveRef,
            .pDepthStencilAttachment = &depthAttachmentRef
    };

    std::array<VkAttachmentDescription, 3> attachments = {colorAttachment,
                                                          depthAttachment,
                                                          colorAttachmentResolve};
    VkRenderPassCreateInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
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

void GraphicsPipeline::createGraphicsPipeline(VkShaderModule& vertShaderModule, VkShaderModule& fragShaderModule) {
    VkPipelineShaderStageCreateInfo shaderStages[2] = {
            { //0
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = vertShaderModule,
                    .pName = "main",
                    .pSpecializationInfo = nullptr
            },
            { //1
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = fragShaderModule,
                    .pName = "main",
                    .pSpecializationInfo = nullptr
            }
    };

    VkPipelineDepthStencilStateCreateInfo depthStencil{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f
    };

    Utils::PipelineConfigInfo configInfo;
    getPipelineConfigInfo(configInfo);
    VkGraphicsPipelineCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,
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

void GraphicsPipeline::createPipelineLayout() {
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

void GraphicsPipeline::getPipelineConfigInfo( Utils::PipelineConfigInfo& configInfo ) {
    static auto bindingDescription = Vertex::getBindingDescription();
    static auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount = (uint32_t) (attributeDescriptions.size()),
            .pVertexAttributeDescriptions = attributeDescriptions.data()
    };
    configInfo.vertexInputInfo = vertexInputInfo;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
    };
    configInfo.inputAssemblyInfo = inputAssemblyInfo;

    VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = (float) mSwapChain->extent().width,
            .height = (float) mSwapChain->extent().height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
    };

    VkRect2D scissor{
            .offset = {0, 0},
            .extent = mSwapChain->extent()
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
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f
    };
    configInfo.rasterizationInfo = rasterizationInfo;

    auto msaaSamples = Utils::getMaxUsableSampleCount(mContext->physicalDevice());
    VkPipelineMultisampleStateCreateInfo multisampleInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = msaaSamples,
            .sampleShadingEnable = VK_TRUE,
            .minSampleShading = .2f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
    };
    configInfo.multisampleInfo = multisampleInfo;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };
    configInfo.colorBlendAttachment = colorBlendAttachment;

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &configInfo.colorBlendAttachment,
            .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
    };
    configInfo.colorBlendInfo = colorBlendInfo;

    configInfo.dynamicStateEnables = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size()),
            .pDynamicStates = configInfo.dynamicStateEnables.data()
    };
    configInfo.dynamicStateInfo = dynamicStateInfo;
}

void GraphicsPipeline::render(GraphicsPipelineRenderInfo& renderInfo) {
    std::vector<VkClearValue> clearValues = {
            {.color = {{0.2f, 0.2f, 0.2f, 1.0f}},},
            {.depthStencil = {1.0f, 0}}
    };

    VkRenderPassBeginInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = mRenderPass,
            .framebuffer = renderInfo.frameBuffer,
            .renderArea = {
                    .offset = {0, 0},
                    .extent = renderInfo.extent
            },
            .clearValueCount = (uint32_t) clearValues.size(),
            .pClearValues = clearValues.data(),
    };
    vkCmdBeginRenderPass(renderInfo.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(renderInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline );

    VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(renderInfo.extent.width),
            .height = static_cast<float>(renderInfo.extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
    };
    vkCmdSetViewport(renderInfo.commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{
            .offset = {0, 0},
            .extent = renderInfo.extent,
    };
    vkCmdSetScissor(renderInfo.commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(renderInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

    VkBuffer vertexBuffers[] = {renderInfo.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(renderInfo.commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(renderInfo.commandBuffer, renderInfo.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(renderInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout,
                            0, 1, &mDescriptorSet->descriptorSets()[renderInfo.currentFrame], 0, nullptr);

    vkCmdDrawIndexed(renderInfo.commandBuffer, renderInfo.indexCount, 1, 0, 0, 0);

    vkCmdEndRenderPass(renderInfo.commandBuffer);
}