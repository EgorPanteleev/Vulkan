//
// Created by auser on 3/23/25.
//

#include "SyncObjects.h"
#include "MessageLogger.h"

#include <stdexcept>

SyncObjects::SyncObjects(Context* context, SwapChain* swapChain): mContext(context), mSwapChain(swapChain) {
    createSyncObjects();
}

SyncObjects::~SyncObjects() {
    for ( auto imageAvailableSemaphore: mImageAvailableSemaphores )
        vkDestroySemaphore(mContext->device(), imageAvailableSemaphore, nullptr);
    for ( auto renderFinishedSemaphore: mRenderFinishedSemaphores )
        vkDestroySemaphore(mContext->device(), renderFinishedSemaphore, nullptr);
    for ( auto inFlightFence: mInFlightFences )
        vkDestroyFence(mContext->device(), inFlightFence, nullptr);
}

void SyncObjects::createSyncObjects() {
    int maxFramesInFlight = mContext->maxFramesInFlight();
    mImageAvailableSemaphores.resize( maxFramesInFlight );
    mRenderFinishedSemaphores.resize( mSwapChain->images().size() );
    mInFlightFences.resize( maxFramesInFlight );

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    auto logicalDevice = mContext->device();

    for ( auto& imageAvailableSemaphore: mImageAvailableSemaphores ){
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore) == VK_SUCCESS) continue;
        throw std::runtime_error("Failed to create image available semaphore for a frame!");
    }
    for ( auto& renderFinishedSemaphore: mRenderFinishedSemaphores ) {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore) == VK_SUCCESS) continue;
        throw std::runtime_error("Failed to create render finished semaphore for a frame!");
    }
    for ( auto& inFlightFence: mInFlightFences ) {
        if (vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFence) == VK_SUCCESS) continue;
        throw std::runtime_error("Failed to create in flight fence for a frame!");
    }
    INFO << "Created synchronization objects!";
}
