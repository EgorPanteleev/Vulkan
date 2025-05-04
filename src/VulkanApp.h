//
// Created by auser on 5/4/25.
//

#ifndef VULKAN_VULKANAPP_H
#define VULKAN_VULKANAPP_H

#include "Renderer.h"

class VulkanApp {
public:
    VulkanApp();
    void run();
private:
    Renderer renderer;

};


#endif //VULKAN_VULKANAPP_H
