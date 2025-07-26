//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include <memory>

#include "Context.h"
#include "Image.h"

class SwapChain {
public:
    SwapChain(Context* context);
    ~SwapChain();
    void recreate();
    void clear();
    /**
     *  Getters
     */
    VkSwapchainKHR swapChain() { return mSwapChain; }
    VkFormat format() const { return mFormat; }
    VkExtent2D extent() const { return mExtent; }
    const std::vector<std::unique_ptr<Image>>& images() const { return mImages; }
    std::vector<VkFramebuffer>& imGuiFrameBuffers() { return mImGuiFrameBuffers; }
    uint32_t imageIndex() const { return mImageIndex; }
    /**
    * Work with images
    */
    VkResult acquireNextImage(VkSemaphore imageAvailableSemaphore, VkFence inFlightFence);
    /**
    * Creating frame buffers
    */
    void createImGuiFrameBuffers(VkRenderPass renderPass);
private:
    /**
     *  Creating swapchain
     */
    void createSwapChain();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    /**
     * Creating images and image views
     */
    void createImages();

    Context* mContext;
    VkSwapchainKHR mSwapChain;
    VkFormat mFormat;
    VkExtent2D mExtent;
    std::vector<std::unique_ptr<Image>> mImages;
    std::vector<VkFramebuffer> mImGuiFrameBuffers;
    uint32_t mImageIndex;
};


#endif //VULKAN_SWAPCHAIN_H
