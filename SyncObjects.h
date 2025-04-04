//
// Created by auser on 3/23/25.
//

#ifndef VULKAN_SYNCOBJECTS_H
#define VULKAN_SYNCOBJECTS_H

#include "Context.h"

class SyncObjects {
public:
    SyncObjects( Context* context );
    ~SyncObjects();

    VkSemaphore imageAvailableSemaphore(uint32_t frameIndex) { return mImageAvailableSemaphores[frameIndex]; }

    VkSemaphore renderFinishedSemaphore(uint32_t frameIndex) { return mRenderFinishedSemaphores[frameIndex]; }

    VkFence& inFlightFence(uint32_t frameIndex) { return mInFlightFences[frameIndex]; }
private:
    void createSyncObjects();

    Context* mContext;
    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFences;
};


#endif //VULKAN_SYNCOBJECTS_H
