//
// Created by auser on 3/23/25.
//

#include "SyncObjects.h"

#include <stdexcept>

SyncObjects::SyncObjects( Device& device ): device( device ) {
    createSyncObjects();
}

SyncObjects::~SyncObjects() {
    vkDestroySemaphore(device.getLogicalDevice(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device.getLogicalDevice(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(device.getLogicalDevice(), inFlightFence, nullptr);
}

void SyncObjects::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateSemaphore(device.getLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device.getLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(device.getLogicalDevice(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphores!");
    }
}
