//
// Created by auser on 3/23/25.
//

#include "SyncObjects.h"
#include "MessageLogger.h"

#include <stdexcept>

SyncObjects::SyncObjects( Context* context ): mContext(context) {
    createSyncObjects();
}

SyncObjects::~SyncObjects() {
    for (size_t i = 0; i < mContext->maxFramesInFlight(); i++) {
        vkDestroySemaphore(mContext->device(), mRenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(mContext->device(), mImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(mContext->device(), mInFlightFences[i], nullptr);
    }
}

void SyncObjects::createSyncObjects() {
    int maxFramesInFlight = mContext->maxFramesInFlight();
    mImageAvailableSemaphores.resize( maxFramesInFlight );
    mRenderFinishedSemaphores.resize( maxFramesInFlight );
    mInFlightFences.resize( maxFramesInFlight );

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    auto logicalDevice = mContext->device();
    for ( size_t i = 0; i < maxFramesInFlight; ++i ) {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("Failed to create synchronization objects for a frame!");
        }
    }
    INFO << "Created synchronization objects!";
}
