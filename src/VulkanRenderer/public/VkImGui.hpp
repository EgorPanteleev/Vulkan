//
// Created by auser on 6/18/25.
//

#ifndef VULKAN_VKIMGUI_H
#define VULKAN_VKIMGUI_H

#include "Context.hpp"
#include "SwapChain.hpp"
#include "GraphicsPipeline.hpp"
#include <imgui.h>

class VkImGui {
public:
    VkImGui(Context* context, SwapChain* swapChain);
    ~VkImGui();

    void beginFrame();
    void endFrame();
    void render(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    static void demo();

    static bool selectableButton(const char* label, bool cond);

    ImDrawData* drawData() { return mDrawData; }
    VkRenderPass renderPass() { return mRenderPass; }

private:
    void createDesriptorPool();
    void createRenderPass();
    void setupStyle();

    Context* mContext;
    SwapChain* mSwapChain;

    VkDescriptorPool mDescriptorPool;
    VkRenderPass mRenderPass;

    ImDrawData* mDrawData;
};


#endif //VULKAN_VKIMGUI_H
