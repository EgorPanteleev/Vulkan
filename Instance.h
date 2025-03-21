//
// Created by auser on 3/21/25.
//

#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class Instance {
public:
    Instance();
    ~Instance();

private:
    void createInstance();
    std::vector<const char *> getRequiredExtensions();
    void hasGflwRequiredInstanceExtensions();
    VkInstance instance;
};


#endif //VULKAN_INSTANCE_H
