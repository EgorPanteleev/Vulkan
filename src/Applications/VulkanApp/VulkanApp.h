//
// Created by auser on 6/19/25.
//

#ifndef VULKAN_VULKANAPP_H
#define VULKAN_VULKANAPP_H

#include "Renderer.h"
#include "MessageLogger.h"

class VulkanApp {
public:
    VulkanApp();
    void run();
private:
    void setCallBacks();

    Renderer mRenderer;
};


#endif //VULKAN_VULKANAPP_H
