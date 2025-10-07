//
// Created by auser on 5/18/25.
//

#ifndef VULKAN_MODELUNIFORMBUFFER_H
#define VULKAN_MODELUNIFORMBUFFER_H

#include "UniformBuffer.hpp"

struct alignas(16) UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class ModelUniformBuffer: public UniformBuffer {
public:
    ModelUniformBuffer(Context* context, cs::AbsCamera* camera);
    virtual ~ModelUniformBuffer() {};

    void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) override;
    VkDeviceSize getSize() const override { return sizeof(UniformBufferObject); }
private:
};


#endif //VULKAN_MODELUNIFORMBUFFER_H
