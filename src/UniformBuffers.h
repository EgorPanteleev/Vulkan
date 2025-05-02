//
// Created by auser on 4/5/25.
//

#ifndef VULKAN_UNIFORMBUFFER_H
#define VULKAN_UNIFORMBUFFER_H

#include <glm/glm.hpp>
#include "Context.h"

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class UniformBuffers {
public:
    UniformBuffers(Context* context);
    ~UniformBuffers();
    void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent);

    std::vector<VkBuffer>& uniformBuffers() { return mUniformBuffers; }
private:
    void createUniformBuffers();

    Context* mContext;
    std::vector<VkBuffer> mUniformBuffers;
    std::vector<VmaAllocation> mBuffersAllocation;
    std::vector<void*> mUniformBuffersMapped;
};


#endif //VULKAN_UNIFORMBUFFER_H
