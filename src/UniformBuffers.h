//
// Created by auser on 4/5/25.
//

#ifndef VULKAN_UNIFORMBUFFER_H
#define VULKAN_UNIFORMBUFFER_H

#include <glm/glm.hpp>
#include "Context.h"
#include "Camera.h"

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

//struct CameraUBO {
//    glm::mat4 view;
//    glm::mat4 proj;
//};
//
//// binding = 1 : Light UBO
//struct LightUBO {
//    glm::vec4 lightPos;   // w unused
//    glm::vec4 lightColor; // rgb,  a unused
//};
//
//// binding = 2 : Material UBO
//struct MaterialUBO {
//    glm::vec4 ambient;   // rgb ambient color
//    glm::vec4 diffuse;   // rgb diffuse color
//    glm::vec4 specular;  // rgb specular color
//    float shininess;
//    // pad to vec4
//    float pad[3];
//};

class UniformBuffers {
public:
    UniformBuffers(Context* context, Camera* camera);
    ~UniformBuffers();
    void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent);

    std::vector<VkBuffer>& uniformBuffers() { return mUniformBuffers; }
private:
    void createUniformBuffers();

    Context* mContext;
    Camera* mCamera;
    std::vector<VkBuffer> mUniformBuffers;
    std::vector<VmaAllocation> mBuffersAllocation;
    std::vector<void*> mUniformBuffersMapped;
};


#endif //VULKAN_UNIFORMBUFFER_H
