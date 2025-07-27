//
// Created by auser on 4/5/25.
//

#ifndef VULKAN_UNIFORMBUFFER_H
#define VULKAN_UNIFORMBUFFER_H

//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLM_FORCE_RIGHT_HANDED
#include <glm/glm.hpp>
#include "Context.hpp"
#include "Camera.hpp"

class UniformBuffer {
public:
    UniformBuffer(Context* context, AbsCamera* camera);
    virtual ~UniformBuffer();
    virtual void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) = 0;

    std::vector<VkBuffer>& uniformBuffers() { return mUniformBuffers; }

    void setCamera(AbsCamera* camera) { mCamera = camera; }

    virtual VkDeviceSize getSize() const = 0;
protected:
    void createUniformBuffers();

    Context* mContext;
    AbsCamera* mCamera;
    std::vector<VkBuffer> mUniformBuffers;
    std::vector<VmaAllocation> mBuffersAllocation;
    std::vector<void*> mUniformBuffersMapped;
};


#endif //VULKAN_UNIFORMBUFFER_H
