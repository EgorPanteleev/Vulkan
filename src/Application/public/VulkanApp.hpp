//
// Created by auser on 6/19/25.
//

#ifndef VULKAN_VULKANAPP_H
#define VULKAN_VULKANAPP_H

#include "Renderer.hpp"
#include "Message.hpp"

class VulkanApp {
public:
    VulkanApp(const std::string& modelPath, const std::vector<std::string>& skyBoxPaths, cs::CameraCreateInfo& cameraCreateInfo);
    void run();
private:
    void setCallBacks();

    Renderer mRenderer;
};


#endif //VULKAN_VULKANAPP_H
