//
// Created by auser on 4/4/25.
//

#include "SwapChain.h"
#include "Utils.h"

SwapChain::SwapChain(Context* context):
                     mContext(context), mCurrentFrame(0) {
    recreate();
}

SwapChain::~SwapChain() {
    clear();
}

void SwapChain::recreate() {
    createSwapChain();
    createImages();
    createImageViews();
}

void SwapChain::clear() {
    for ( auto framebuffer : mFrameBuffers ) {
        vkDestroyFramebuffer( mContext->device(), framebuffer, nullptr );
    }
    for ( auto framebuffer : mShadowFrameBuffers ) {
        vkDestroyFramebuffer( mContext->device(), framebuffer, nullptr );
    }
    vkDestroySwapchainKHR(mContext->device(), mSwapChain, nullptr);
    for (auto imageView : mImageViews) {
        vkDestroyImageView(mContext->device(), imageView, nullptr);
    }
    mImageViews.clear();
}

void SwapChain::updateCurrentFrame() {
    mCurrentFrame = (mCurrentFrame + 1) % mContext->maxFramesInFlight();
}

void SwapChain::createSwapChain() {
    auto swapChainSupport =
            Utils::getSwapChainSupport(mContext->physicalDevice(), mContext->surface());

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    auto imageCount = Utils::getImageCount(swapChainSupport);

    mFormat = surfaceFormat.format;
    mExtent = extent;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = mContext->surface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto indices = Utils::getQueueFamilies(mContext->physicalDevice(), mContext->surface() );
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(mContext->device(), &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain!");
    }
    INFO << "Created swapchain!";
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }
    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        mContext->window().getFrameBufferSize( width, height );

        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void SwapChain::createImages() {
    auto swapChainSupport =
            Utils::getSwapChainSupport(mContext->physicalDevice(), mContext->surface());
    auto imageCount = Utils::getImageCount(swapChainSupport);
    vkGetSwapchainImagesKHR(mContext->device(), mSwapChain, &imageCount, nullptr);
    mImages.resize(imageCount);
    vkGetSwapchainImagesKHR(mContext->device(), mSwapChain, &imageCount, mImages.data());
}

void SwapChain::createImageViews() {
    mImageViews.resize(mImages.size());
    for (size_t i = 0; i < mImages.size(); ++i) {
        mImageViews[i] = Utils::createImageView(mContext->device(), mImages[i], 1, VK_IMAGE_VIEW_TYPE_2D,
                                                mFormat, VK_IMAGE_ASPECT_COLOR_BIT );
    }
}


VkResult SwapChain::acquireNextImage(uint32_t* imageIndex, VkSemaphore imageAvailableSemaphore, VkFence inFlightFence) {
    vkWaitForFences(mContext->device(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(mContext->device(), 1, &inFlightFence);

    VkResult result = vkAcquireNextImageKHR(mContext->device(), mSwapChain, UINT64_MAX,
                                             imageAvailableSemaphore,
                                             VK_NULL_HANDLE, imageIndex);

    return result;
}

void SwapChain::createFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView, VkImageView colorImageView) {
    mFrameBuffers.resize( mImageViews.size() );
    for (size_t i = 0; i < mImageViews.size(); ++i) {
        std::vector<VkImageView> attachments = {
                colorImageView,
                depthImageView,
                mImageViews[i]
        };
        mFrameBuffers[i] = Utils::createFrameBuffer(mContext->device(), renderPass, attachments, mExtent);
    }
    INFO << "Created frame buffers!";
}

void SwapChain::createShadowFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView) {
    size_t shadowSize = 1;
    mShadowFrameBuffers.resize( shadowSize );
    VkExtent2D shadowExtent = {1024, 1024};
    for (size_t i = 0; i < shadowSize; ++i) {
        std::vector<VkImageView> attachments = {
                depthImageView
        };
        mShadowFrameBuffers[i] = Utils::createFrameBuffer(mContext->device(), renderPass, attachments, shadowExtent);
    }
}

