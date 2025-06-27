//
// Created by auser on 6/18/25.
//

#ifndef VULKAN_VKIMGUI_H
#define VULKAN_VKIMGUI_H

#include "Context.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include <imgui.h>

class VkImGui {
public:
    VkImGui(Context* context, SwapChain* swapChain);
    ~VkImGui();

    void beginFrame();
    void endFrame();
    void render(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    static void demo();

    ImDrawData* drawData() { return mDrawData; }
    VkRenderPass renderPass() { return mRenderPass; }

private:
    void createDesriptorPool();
    void createRenderPass();

    Context* mContext;
    SwapChain* mSwapChain;

    VkDescriptorPool mDescriptorPool;
    VkRenderPass mRenderPass;

    ImDrawData* mDrawData;
};


#endif //VULKAN_VKIMGUI_H
