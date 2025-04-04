//
// Created by auser on 3/23/25.
//

#include "FrameBuffers.h"
#include "stdexcept"

FrameBuffers::FrameBuffers(Device& device, SwapChain_old& swapChain, RenderPass& renderPass ):
                            device( device ), swapChain( swapChain ), renderPass( renderPass )  {
    createFrameBuffers();
}

FrameBuffers::~FrameBuffers() {
    for ( auto framebuffer : frameBuffers ) {
        vkDestroyFramebuffer( device.getLogicalDevice(), framebuffer, nullptr );
    }
}

void FrameBuffers::createFrameBuffers() {
    auto swapChainImageViews = swapChain.getImageViews();
    auto swapChainExtent = swapChain.getExtent();
    frameBuffers.resize( swapChainImageViews.size() );
    for (size_t i = 0; i < swapChainImageViews.size(); ++i) {
        VkImageView attachments[] = { swapChainImageViews[i] };
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device.getLogicalDevice(), &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create frame buffer!");
        }
    }
}