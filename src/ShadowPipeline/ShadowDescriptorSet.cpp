//
// Created by auser on 5/19/25.
//

#include "ShadowDescriptorSet.h"
#include "MessageLogger.h"

ShadowDescriptorSet::ShadowDescriptorSet(ShadowDescriptorSetCreateInfo& createInfo):
                                   mContext(createInfo.context),
                                   mUniformBuffers(createInfo.uniformBuffers) {
    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();
    updateDescriptorSets();
}

ShadowDescriptorSet::~ShadowDescriptorSet() {
    vkDestroyDescriptorPool(mContext->device(), mDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mContext->device(), mDescriptorSetLayout, nullptr);
}

void ShadowDescriptorSet::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding directLightBinding{
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
    };

    VkDescriptorSetLayoutBinding MVPBinding{
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr
    };

    std::vector<VkDescriptorSetLayoutBinding> bindings = {directLightBinding,
                                                          MVPBinding        };
    VkDescriptorSetLayoutCreateInfo layoutInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = (uint32_t)bindings.size(),
            .pBindings = bindings.data()
    };

    if (vkCreateDescriptorSetLayout(mContext->device(), &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
    INFO << "Created descriptor set layout!";
}

void ShadowDescriptorSet::createDescriptorPool() {
    auto descriptorCount = (uint32_t)mContext->maxFramesInFlight();
    std::vector<VkDescriptorPoolSize> poolSizes{
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER        , descriptorCount},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER        , descriptorCount}
    };

    VkDescriptorPoolCreateInfo poolInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = (uint32_t)mContext->maxFramesInFlight(),
            .poolSizeCount = (uint32_t)poolSizes.size(),
            .pPoolSizes = poolSizes.data()
    };

    if (vkCreateDescriptorPool(mContext->device(), &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
    INFO << "Created descriptor pool!";
}

void ShadowDescriptorSet::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(mContext->maxFramesInFlight(), mDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = mDescriptorPool,
            .descriptorSetCount = (uint32_t)mContext->maxFramesInFlight(),
            .pSetLayouts = layouts.data()
    };
    mDescriptorSets.resize(mContext->maxFramesInFlight());

    if (vkAllocateDescriptorSets(mContext->device(), &allocInfo, mDescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }
    INFO << "Created descriptor sets!";
}

void ShadowDescriptorSet::updateDescriptorSets() {
    for (size_t i = 0; i < mContext->maxFramesInFlight(); ++i) {
        VkDescriptorBufferInfo directLightBufferInfo{
                .buffer = mUniformBuffers[2]->uniformBuffers()[i],
                .offset = 0,
                .range = mUniformBuffers[2]->getSize()
        };

        VkDescriptorBufferInfo MVPBufferInfo{
                .buffer = mUniformBuffers[0]->uniformBuffers()[i],
                .offset = 0,
                .range = mUniformBuffers[0]->getSize()
        };

        std::vector<VkWriteDescriptorSet> descriptorWrites{
                {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = mDescriptorSets[i],
                        .dstBinding = 0,
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .pImageInfo = nullptr,
                        .pBufferInfo = &directLightBufferInfo,
                        .pTexelBufferView = nullptr
                },
                {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = mDescriptorSets[i],
                        .dstBinding = 1,
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .pImageInfo = nullptr,
                        .pBufferInfo = &MVPBufferInfo,
                        .pTexelBufferView = nullptr
                },
        };

        vkUpdateDescriptorSets(mContext->device(), (uint32_t)descriptorWrites.size(),
                               descriptorWrites.data(), 0, nullptr);
    }
}
