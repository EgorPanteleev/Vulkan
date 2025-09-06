//
// Created by igor on 8/25/25.
//

#ifndef VULKAN_CUBEMAPDESCRIPTORSET_HPP
#define VULKAN_CUBEMAPDESCRIPTORSET_HPP

#include "Context.hpp"
#include "UniformBuffer.hpp"
#include "Texture.hpp"
#include "VulkanModelLoader.hpp"

using UniformBuffers = std::vector<std::unique_ptr<UniformBuffer>>;

struct CubeMapDescriptorSetCreateInfo {
    Context* context;
    VulkanModelLoader* loader;
    const UniformBuffers& uniformBuffers;
};

class CubeMapDescriptorSet {
public:

    CubeMapDescriptorSet(CubeMapDescriptorSetCreateInfo& createInfo);
    ~CubeMapDescriptorSet();

    VkDescriptorSetLayout& descriptorSetLayout() { return mDescriptorSetLayout; }

    std::vector<VkDescriptorSet>& descriptorSets() { return mDescriptorSets; }

    void updateDescriptorSets();

private:
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();

    Context* mContext;

    VulkanModelLoader* mLoader;

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

#endif //VULKAN_CUBEMAPDESCRIPTORSET_HPP
