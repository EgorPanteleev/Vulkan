//
// Created by auser on 3/23/25.
//

#ifndef VULKAN_SYNCOBJECTS_H
#define VULKAN_SYNCOBJECTS_H

#include "Device.h"

class SyncObjects {
public:
    SyncObjects( Device& device );
    ~SyncObjects();

    VkSemaphore getImageAvailableSemaphore() { return imageAvailableSemaphore; }

    VkSemaphore getRenderFinishedSemaphore() { return renderFinishedSemaphore; }

    VkFence& getInFlightFence() { return inFlightFence; }
private:
    void createSyncObjects();

    Device& device;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
};


#endif //VULKAN_SYNCOBJECTS_H
