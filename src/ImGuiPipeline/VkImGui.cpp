//
// Created by auser on 6/18/25.
//

#include "VkImGui.h"
#include "Utils.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

VkImGui::VkImGui(Context* context, SwapChain* swapChain):
           mContext(context), mSwapChain(swapChain) {
    createDesriptorPool();
    createRenderPass();

    //context setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //init glfw backend
    bool installCallbacks = false;
    ImGui_ImplGlfw_InitForVulkan(mContext->window().window(), installCallbacks);

    //init vulkan backend
    auto indices = Utils::getQueueFamilies(mContext->physicalDevice(), mContext->surface());
    ImGui_ImplVulkan_InitInfo init_info = {
            .Instance = mContext->instance(),
            .PhysicalDevice = mContext->physicalDevice(),
            .Device = mContext->device(),
            .QueueFamily = indices.graphicsFamily.value(),
            .Queue = mContext->graphicsQueue(),
            .DescriptorPool = mDescriptorPool,
            .RenderPass = mRenderPass,
            .MinImageCount = (uint32_t)mSwapChain->images().size(),
            .ImageCount = (uint32_t)mSwapChain->images().size(),
            .PipelineCache = VK_NULL_HANDLE
    };

    ImGui_ImplVulkan_Init(&init_info);

    ImGui_ImplVulkan_CreateFontsTexture();

    //upload fonts
//    auto commandPool = Utils::createCommandPool(mContext, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT );
//    VkCommandBuffer commandBuffer = Utils::beginSingleTimeCommands(mContext->device(), commandPool);
//    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
//    Utils::endSingleTimeCommands(mContext, commandPool, commandBuffer);
//    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

VkImGui::~VkImGui() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyRenderPass(mContext->device(), mRenderPass, nullptr);
    vkDestroyDescriptorPool(mContext->device(), mDescriptorPool, nullptr);
}

void VkImGui::createDesriptorPool() {
    uint32_t size = 10;
    VkDescriptorPoolSize pool_sizes[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLER, size },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, size },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, size },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, size },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, size },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, size },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, size },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, size },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, size },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, size },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, size }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 10 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
    pool_info.pPoolSizes = pool_sizes;

    vkCreateDescriptorPool(mContext->device(), &pool_info, nullptr, &mDescriptorPool);
}

void VkImGui::createRenderPass() {
    VkAttachmentDescription colorAttachment {
            .format = mSwapChain->format(),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorAttachmentRef{
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDependency dependency{
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    };

    VkSubpassDescription subpass{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentRef,
    };

    std::vector<VkAttachmentDescription> attachments = {colorAttachment};
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

void VkImGui::beginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void VkImGui::endFrame() {
    ImGui::Render();
    mDrawData = ImGui::GetDrawData();
}

void VkImGui::demo() {
    ImGui::ShowDemoWindow(nullptr);
}

void VkImGui::render(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkClearValue clearColor{
        .color = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}}
    };
    VkRenderPassBeginInfo renderPassBeginInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = mRenderPass,
            .framebuffer = mSwapChain->imGuiFrameBuffers()[imageIndex],
            .renderArea = {
                    .offset = {0, 0},
                    .extent = mSwapChain->extent()
            },
            .clearValueCount = 1,
            .pClearValues = &clearColor
    };
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    ImGui_ImplVulkan_RenderDrawData(mDrawData, commandBuffer);
    vkCmdEndRenderPass(commandBuffer);
}