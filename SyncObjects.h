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

    VkSemaphore getImageAvailableSemaphore( uint32_t frameIndex ) { return imageAvailableSemaphores[frameIndex]; }

    VkSemaphore getRenderFinishedSemaphore( uint32_t frameIndex ) { return renderFinishedSemaphores[frameIndex]; }

    VkFence& getInFlightFence( uint32_t frameIndex ) { return inFlightFences[frameIndex]; }
private:
    void createSyncObjects();

    Device& device;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
};


#endif //VULKAN_SYNCOBJECTS_H
