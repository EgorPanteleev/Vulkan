//
// Created by auser on 5/19/25.
//

#ifndef VULKAN_SHADOWDESCRIPTORSET_H
#define VULKAN_SHADOWDESCRIPTORSET_H

#include "Context.h"
#include "UniformBuffer.h"
#include "DepthResources.h"

class ShadowDescriptorSet {
public:
    using UniformBuffers = std::vector<std::unique_ptr<UniformBuffer>>;

    ShadowDescriptorSet(Context* context, DepthResources* depthResources,
                  const UniformBuffers& uniformBuffers);
    ~ShadowDescriptorSet();

    VkDescriptorSetLayout& descriptorSetLayout() { return mDescriptorSetLayout; }

    std::vector<VkDescriptorSet>& descriptorSets() { return mDescriptorSets; }

private:
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();
    void updateDescriptorSets();


    Context* mContext;

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

#endif //VULKAN_SHADOWDESCRIPTORSET_H
