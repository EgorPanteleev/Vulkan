//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include "Context.h"
#include "DepthResources.h"

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
    VkFormat format() { return mFormat; }
    VkExtent2D extent() { return mExtent; }
    std::vector<VkImage>& images() { return mImages; }
    std::vector<VkImageView>& imageViews() { return mImageViews; }
    uint32_t currentFrame() { return mCurrentFrame; }
    std::vector<VkFramebuffer>& frameBuffers() { return mFrameBuffers; }
    std::vector<VkFramebuffer>& shadowFrameBuffers() { return mShadowFrameBuffers; }
    /**
    * Work with images
    */
    VkResult acquireNextImage(uint32_t* imageIndex, VkSemaphore imageAvailableSemaphore, VkFence inFlightFence);
    void updateCurrentFrame();
    /**
    * Creating frame buffers
    */
    void createFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView, VkImageView colorImageView);
    void createShadowFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView);
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
    void createImageViews();

    Context* mContext;
    VkSwapchainKHR mSwapChain;
    VkFormat mFormat;
    VkExtent2D mExtent;
    std::vector<VkImage> mImages;
    std::vector<VkImageView> mImageViews;
    uint32_t mCurrentFrame;
    std::vector<VkFramebuffer> mFrameBuffers;
    std::vector<VkFramebuffer> mShadowFrameBuffers;
};


#endif //VULKAN_SWAPCHAIN_H
