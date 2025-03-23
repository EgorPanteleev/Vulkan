//
// Created by auser on 3/23/25.
//

#include "SyncObjects.h"

#include <stdexcept>

SyncObjects::SyncObjects( Device& device ): device( device ) {
    createSyncObjects();
}

SyncObjects::~SyncObjects() {
    for (size_t i = 0; i < device.getMaxFramesInFlight(); i++) {
        vkDestroySemaphore(device.getLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device.getLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device.getLogicalDevice(), inFlightFences[i], nullptr);
    }
}

void SyncObjects::createSyncObjects() {
    int maxFramesInFlight = device.getMaxFramesInFlight();
    imageAvailableSemaphores.resize( maxFramesInFlight );
    renderFinishedSemaphores.resize( maxFramesInFlight );
    inFlightFences.resize( maxFramesInFlight );

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    auto logicalDevice = device.getLogicalDevice();
    for ( size_t i = 0; i < maxFramesInFlight; ++i ) {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("Failed to create synchronization objects for a frame!");
        }
    }
}
