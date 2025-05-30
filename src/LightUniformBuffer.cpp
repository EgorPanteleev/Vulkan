//
// Created by auser on 5/18/25.
//

#include "LightUniformBuffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>

LightUniformBuffer::LightUniformBuffer(Context* context, Camera* camera):
                                       UniformBuffer(context, camera), mViewPos(camera->position()){
    createUniformBuffers();
}

void LightUniformBuffer::updateUniformBuffer(uint32_t currentImage, VkExtent2D extent) {
    Light lightData = {};
    lightData.viewPos = glm::vec4(mViewPos, 1);
    lightData.lightCount = 1;  // Example: 3 lights

// Light 1: Red
    lightData.lightPos[0] = glm::vec4(0.0, 0.0, 0.0, 1);
    lightData.lightColor[0] = glm::vec4(1.0, 1.0, 1.0, 1.0);
    //lightData.lightViewProj[0] = {};

//// Light 2: Green
//    lightData.lightPos[1] = glm::vec4(-1.0, -2.0, 1.0, 0.0);
//    lightData.lightColor[1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0);
//
//// Light 3: Blue
//    lightData.lightPos[2] = glm::vec4(0.0f, 1.0f, 3.0f, 0.0);
//    lightData.lightColor[2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0);
//    std::memcpy(mUniformBuffersMapped[currentImage], &lightData, getSize());
}