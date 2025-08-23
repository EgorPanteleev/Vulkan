//
// Created by auser on 6/18/25.
//

#include "VkImGui.hpp"
#include "Utils.hpp"
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
    io.MouseDrawCursor = true;
    setupStyle();

    //init glfw backend
    bool installCallbacks = false;
    ImGui_ImplGlfw_InitForVulkan(mContext->window().window(), installCallbacks);

    //init vulkan backend
    ImGui_ImplVulkan_InitInfo init_info = {
            .Instance = mContext->instance(),
            .PhysicalDevice = mContext->physicalDevice(),
            .Device = mContext->device(),
            .QueueFamily = mContext->familyIndices().graphicsFamily.value(),
            .Queue = mContext->graphicsQueue(),
            .DescriptorPool = mDescriptorPool,
            .RenderPass = mRenderPass,
            .MinImageCount = (uint32_t)mSwapChain->images().size(),
            .ImageCount = (uint32_t)mSwapChain->images().size(),
            .PipelineCache = VK_NULL_HANDLE
    };

    ImGui_ImplVulkan_Init(&init_info);

    //ImGui_ImplVulkan_CreateFontsTexture();
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

void VkImGui::setupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    ImGuiIO& io = ImGui::GetIO();

    colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
    colors[ImGuiCol_Border]                = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.05f, 0.05f, 0.05f, 0.75f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.30f, 0.30f, 0.30f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.35f, 0.35f, 0.35f, 0.90f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.56f, 0.90f, 0.69f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.42f, 0.75f, 0.58f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.36f, 0.85f, 0.58f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_Separator]             = ImVec4(0.35f, 0.35f, 0.35f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.55f, 0.55f, 0.55f, 0.78f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.30f, 0.30f, 0.30f, 0.30f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.35f, 0.35f, 0.35f, 0.60f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.24f, 0.54f, 0.33f, 0.45f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(0.90f, 0.30f, 0.30f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = ImVec4(0.42f, 0.75f, 0.58f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.15f, 0.15f, 0.15f, 0.40f);

    style.WindowRounding    = 6.0f;
    style.FrameRounding     = 4.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding      = 3.0f;
    style.TabRounding       = 4.0f;

    io.Fonts->AddFontFromFileTTF(PROJECT_PATH"assets/fonts/JetBrainsMono-Regular.ttf", 15.0f);
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

bool VkImGui::selectableButton(const char* label, bool cond) {
    if (cond) {
        ImVec4 baseColor = {0.4f, 0.4f, 0.4f, 1.0f};
        ImGui::PushStyleColor(ImGuiCol_Button,        baseColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(baseColor.x + 0.1f, baseColor.y + 0.1f, baseColor.z + 0.1f, baseColor.w));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(baseColor.x - 0.1f, baseColor.y - 0.1f, baseColor.z - 0.1f, baseColor.w));
    }
    bool ret = ImGui::Button(label) && !cond;
    if (cond) ImGui::PopStyleColor(3);

    return ret;
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