//
// Created by auser on 3/22/25.
//

#include "GraphicsPipeline.hpp"
#include "Utils.hpp"


GraphicsPipeline::GraphicsPipeline(GraphicsPipelineCreateInfo& createInfo):
                                   mContext(createInfo.context), mSwapChain(createInfo.swapChain),
                                   mColorBuffer(new Image(mContext)), mDepthBuffer(new Image(mContext)),
                                   mEnableMSAA(createInfo.enableMSAA){
    createColorBuffer();
    createDepthBuffer();
    createDescriptorSet(createInfo);
    createPipelineLayout();
    createGraphicsPipeline( createInfo.vertShaderPath, createInfo.fragShaderPath );
}

GraphicsPipeline::~GraphicsPipeline(){
    vkDestroyPipeline(mContext->device(), mPipeline, nullptr);
    vkDestroyShaderModule( mContext->device(), mVertShader, nullptr );
    vkDestroyShaderModule( mContext->device(), mFragShader, nullptr );
    vkDestroyPipelineLayout(mContext->device(), mPipelineLayout, nullptr);
    vkDestroyPipelineCache(mContext->device(), mPipelineCache, nullptr);
    delete mDescriptorSet;
    mDepthBuffer->destroy();
    delete mDepthBuffer;
    mColorBuffer->destroy();
    delete mColorBuffer;
}

void GraphicsPipeline::createColorBuffer() {
    ImageAllocateInfo allocateInfo{
            .format = mSwapChain->format(),
            .extent = mSwapChain->extent(),
            .numSamples = getNumSamples(),
            .imageUsageFlags = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
    };
    mColorBuffer->allocate(allocateInfo);
    ImageTransitInfo transitInfo{
        .src = VK_IMAGE_LAYOUT_UNDEFINED,
        .dst = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    mColorBuffer->transit(transitInfo);
}

void GraphicsPipeline::createDepthBuffer() {
    ImageAllocateInfo allocateInfo{
            .format = Utils::findDepthFormat(mContext),
            .extent = mSwapChain->extent(),
            .numSamples = getNumSamples(),
            .imageUsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT,
    };
    mDepthBuffer->allocate(allocateInfo);
    ImageTransitInfo transitInfo{
            .src = VK_IMAGE_LAYOUT_UNDEFINED,
            .dst = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    mDepthBuffer->transit(transitInfo);
}

void GraphicsPipeline::createDescriptorSet(GraphicsPipelineCreateInfo& createInfo) {
    DescriptorSetCreateInfo descriptorSetCreateInfo{
            .context = createInfo.context,
            .loader = createInfo.loader,
            .shadowMap = createInfo.shadowMap,
            .uniformBuffers = createInfo.uniformBuffers
    };
    mDescriptorSet = new DescriptorSet(descriptorSetCreateInfo);
}

void GraphicsPipeline::createGraphicsPipeline(const std::string& vertPath, const std::string& fragPath) {
    Utils::loadShader(mContext->device(), vertPath, mVertShader);
    Utils::loadShader(mContext->device(), fragPath, mFragShader);
    VkPipelineShaderStageCreateInfo shaderStages[2] = {
            { //0
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = mVertShader,
                    .pName = "main",
                    .pSpecializationInfo = nullptr
            },
            { //1
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = mFragShader,
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

    VkFormat colorFormats[] = { mSwapChain->format() };

    VkPipelineRenderingCreateInfo pipelineRenderingInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = colorFormats,
            .depthAttachmentFormat = VK_FORMAT_D32_SFLOAT,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    Utils::PipelineConfigInfo configInfo;
    getPipelineConfigInfo(configInfo);
    VkGraphicsPipelineCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &pipelineRenderingInfo,
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
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
    };

    VkPipelineCacheCreateInfo cacheCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .initialDataSize = 0,
            .pInitialData = nullptr,
    };
    vkCreatePipelineCache(mContext->device(), &cacheCreateInfo, nullptr, &mPipelineCache);

    if (vkCreateGraphicsPipelines(mContext->device(), mPipelineCache, 1, &pipelineInfo, nullptr, &mPipeline) != VK_SUCCESS) {
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
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f
    };
    configInfo.rasterizationInfo = rasterizationInfo;

    auto msaaSamples = getNumSamples();
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

VkSampleCountFlagBits GraphicsPipeline::getNumSamples() const {
    return mEnableMSAA ? Utils::getMaxUsableSampleCount(mContext->physicalDevice())
                       : VK_SAMPLE_COUNT_1_BIT;
}

void GraphicsPipeline::render(GraphicsPipelineRenderInfo& renderInfo) {
    TracyVkZone(mContext->tracyContext(), renderInfo.commandBuffer, "Graphics render");
    ImageTransitInfoCmd transitInfo{
        .commandBuffer = renderInfo.commandBuffer,
        .src = renderInfo.colorLayoutAttachment.finalLayout,
        .dst = renderInfo.colorLayoutAttachment.initialLayout
    };
    renderInfo.presentImage->transit(transitInfo);
    std::vector<VkClearValue> clearValues = {
            {.color = {{0.2f, 0.2f, 0.2f, 1.0f}},},
            {.depthStencil = {1.0f, 0}}
    };

    VkRenderingAttachmentInfo colorAttachment = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = renderInfo.presentImage->imageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = clearValues[0],
    };
    if (mEnableMSAA) {
        colorAttachment.imageView = mColorBuffer->imageView();
        colorAttachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        colorAttachment.resolveImageView = renderInfo.presentImage->imageView();
        colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    }

    VkRenderingAttachmentInfo depthAttachment = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = mDepthBuffer->imageView(),
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .clearValue = clearValues[1],
    };

    VkRenderingInfo renderingInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {
                    .offset = {0, 0},
                    .extent = renderInfo.extent
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachment,
            .pDepthAttachment = &depthAttachment,
    };

    vkCmdBeginRendering(renderInfo.commandBuffer, &renderingInfo);

    vkCmdBindPipeline(renderInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline );

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

    vkCmdBindDescriptorSets(renderInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout,
                            0, 1, &mDescriptorSet->descriptorSets()[renderInfo.currentFrame], 0, nullptr);

    VkBuffer vertexBuffers[] = {renderInfo.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(renderInfo.commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(renderInfo.commandBuffer, renderInfo.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(renderInfo.commandBuffer, renderInfo.indexCount, 1, 0, 0, 0);

    vkCmdEndRendering(renderInfo.commandBuffer);

    std::swap(transitInfo.src, transitInfo.dst);
    renderInfo.presentImage->transit(transitInfo);
}

void GraphicsPipeline::recreateBuffers() {
    colorBuffer()->destroy();
    createColorBuffer();
    depthBuffer()->destroy();
    createDepthBuffer();
}