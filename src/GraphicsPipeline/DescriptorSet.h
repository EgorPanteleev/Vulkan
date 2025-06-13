//
// Created by auser on 4/7/25.
//

#ifndef VULKAN_DESCRIPTORSET_H
#define VULKAN_DESCRIPTORSET_H

#include "Context.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "DepthResources.h"

class DescriptorSet {
public:
    using UniformBuffers = std::vector<std::unique_ptr<UniformBuffer>>;

    DescriptorSet(Context* context, Texture* texture, DepthResources* depthResources, const UniformBuffers& uniformBuffers);
    ~DescriptorSet();

    VkDescriptorSetLayout& descriptorSetLayout() { return mDescriptorSetLayout; }

    std::vector<VkDescriptorSet>& descriptorSets() { return mDescriptorSets; }

private:
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();

    Context* mContext;

    Texture* mTexture;

    DepthResources* mDepthResources;

    const UniformBuffers& mUniformBuffers;
    /**
     * A descriptor set layout describes what types of resources (e.g., uniform buffers, samplers, storage buffers)
     * your shaders expect, and where (binding points).
     */
    VkDescriptorSetLayout mDescriptorSetLayout;
    /**
     * A descriptor pool allocates descriptor sets — it's Vulkan's way of managing memory for those sets.
     */
    VkDescriptorPool mDescriptorPool;
    /**
     * A descriptor set holds actual resource handles (buffers, textures) that are bound to the shader.
     */
    std::vector<VkDescriptorSet> mDescriptorSets;
};


#endif //VULKAN_DESCRIPTORSET_H
