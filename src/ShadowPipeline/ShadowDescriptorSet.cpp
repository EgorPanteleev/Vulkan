//
// Created by auser on 5/19/25.
//

#include "ShadowDescriptorSet.h"
#include "MessageLogger.h"

ShadowDescriptorSet::ShadowDescriptorSet(Context* context, DepthResources* depthResources,
                                   const UniformBuffers& uniformBuffers):
                                   mContext(context), mDepthResources(depthResources),
                                   mUniformBuffers(uniformBuffers) {
    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();
}

ShadowDescriptorSet::~ShadowDescriptorSet() {
    vkDestroyDescriptorPool(mContext->device(), mDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mContext->device(), mDescriptorSetLayout, nullptr);
}

void ShadowDescriptorSet::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    samplerLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding directLightLayoutBinding{};
    directLightLayoutBinding.binding = 1;
    directLightLayoutBinding.descriptorCount = 1;
    directLightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    directLightLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    directLightLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding modelLayoutBinding{};
    modelLayoutBinding.binding = 2;
    modelLayoutBinding.descriptorCount = 1;
    modelLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    modelLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    modelLayoutBinding.pImmutableSamplers = nullptr;


    std::vector<VkDescriptorSetLayoutBinding> bindings = {directLightLayoutBinding,
                                                          samplerLayoutBinding    ,
                                                          modelLayoutBinding      };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(mContext->device(), &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
    INFO << "Created descriptor set layout!";
}

void ShadowDescriptorSet::createDescriptorPool() {
    auto descriptorCount = static_cast<uint32_t>(mContext->maxFramesInFlight());
    std::vector<VkDescriptorPoolSize> poolSizes{};
    //sampler
    poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount);
    poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER        , descriptorCount);
    poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER        , descriptorCount);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(mContext->maxFramesInFlight());
    if (vkCreateDescriptorPool(mContext->device(), &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
    INFO << "Created descriptor pool!";
}

void ShadowDescriptorSet::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(mContext->maxFramesInFlight(), mDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(mContext->maxFramesInFlight());
    allocInfo.pSetLayouts = layouts.data();
    mDescriptorSets.resize(mContext->maxFramesInFlight());
    if (vkAllocateDescriptorSets(mContext->device(), &allocInfo, mDescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }
    INFO << "Created descriptor sets!";

    for (size_t i = 0; i < mContext->maxFramesInFlight(); ++i) {
        VkDescriptorImageInfo shadowInfo{};
        shadowInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        shadowInfo.imageView = mDepthResources->shadowImageView();
        shadowInfo.sampler = mDepthResources->sampler();

        VkDescriptorBufferInfo directLightBufferInfo{};
        directLightBufferInfo.buffer = mUniformBuffers[2]->uniformBuffers()[i];
        directLightBufferInfo.offset = 0;
        directLightBufferInfo.range = mUniformBuffers[2]->getSize();

        VkDescriptorBufferInfo modelBufferInfo{};
        modelBufferInfo.buffer = mUniformBuffers[0]->uniformBuffers()[i];
        modelBufferInfo.offset = 0;
        modelBufferInfo.range = mUniformBuffers[0]->getSize();

        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = mDescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pImageInfo = &shadowInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = mDescriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &directLightBufferInfo;
        descriptorWrites[1].pImageInfo = nullptr;
        descriptorWrites[1].pTexelBufferView = nullptr;

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = mDescriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo = &modelBufferInfo;
        descriptorWrites[2].pImageInfo = nullptr;
        descriptorWrites[2].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(mContext->device(), static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}