//
// Created by auser on 5/31/25.
//

#ifndef VULKAN_DIRECTIONALLIGHT_H
#define VULKAN_DIRECTIONALLIGHT_H

#include "UniformBuffer.h"

struct alignas(16) DirectionalLight {
    glm::mat4 VPMatrix;
    glm::vec4 color;
    glm::vec4 direction;
    //float shadowBias;
};

class DirectionalLightBuffer: public UniformBuffer {
public:
    DirectionalLightBuffer(Context* context, Camera* camera);
    virtual ~DirectionalLightBuffer() {};

    void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) override;
    VkDeviceSize getSize() const override { return sizeof(DirectionalLight); }
protected:
};


#endif //VULKAN_DIRECTIONALLIGHT_H
