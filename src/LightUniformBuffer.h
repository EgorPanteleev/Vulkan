//
// Created by auser on 5/18/25.
//

#ifndef VULKAN_LIGHTUNIFORMBUFFER_H
#define VULKAN_LIGHTUNIFORMBUFFER_H

#include "UniformBuffer.h"

struct alignas(16) Light {
    glm::mat4 lightViewProj[10];
    glm::vec4 lightPos[10];
    glm::vec4 lightColor[10];
    glm::vec4 viewPos;
    int lightCount;
    int padding[3];
};

class LightUniformBuffer: public UniformBuffer {
public:
    LightUniformBuffer(Context* context, Camera* camera);
    virtual ~LightUniformBuffer() {};

    void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) override;
    VkDeviceSize getSize() const override { return sizeof(Light); }
protected:
    glm::vec3 mViewPos;
};


#endif //VULKAN_LIGHTUNIFORMBUFFER_H
