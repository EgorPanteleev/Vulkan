//
// Created by auser on 4/7/25.
//

#include "DescriptorSet.h"
#include "MessageLogger.h"

DescriptorSet::DescriptorSet(Context* context, Texture* texture, DepthResources* depthResources, const UniformBuffers& uniformBuffers):
                             mContext(context), mTexture(texture), mDepthResources(depthResources), mUniformBuffers(uniformBuffers) {
    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();
}

DescriptorSet::~DescriptorSet() {
    vkDestroyDescriptorPool(mContext->device(), mDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mContext->device(), mDescriptorSetLayout, nullptr);
}

void DescriptorSet::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding lightUBOLayoutBinding{};
    lightUBOLayoutBinding.binding = 2;
    lightUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightUBOLayoutBinding.descriptorCount = 1;
    lightUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    lightUBOLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding shadowSamplerLayoutBinding{};
    shadowSamplerLayoutBinding.binding = 3;
    shadowSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    shadowSamplerLayoutBinding.descriptorCount = 1;
    shadowSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    shadowSamplerLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding directLightLayoutBinding{};
    directLightLayoutBinding.binding = 4;
    directLightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    directLightLayoutBinding.descriptorCount = 1;
    directLightLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    directLightLayoutBinding.pImmutableSamplers = nullptr;


    std::vector<VkDescriptorSetLayoutBinding> bindings = {uboLayoutBinding,
                                                          samplerLayoutBinding,
                                                          lightUBOLayoutBinding,
                                                          shadowSamplerLayoutBinding,
                                                          directLightLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(mContext->device(), &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
    INFO << "Created descriptor set layout!";
}

void DescriptorSet::createDescriptorPool() {
    auto descriptorCount = static_cast<uint32_t>(mContext->maxFramesInFlight());
    std::vector<VkDescriptorPoolSize> poolSizes{};
    poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount);
    poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount);
    poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount);
    poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount);
    poolSizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount);

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

void DescriptorSet::createDescriptorSets() {
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
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = mUniformBuffers[0]->uniformBuffers()[i];
        bufferInfo.offset = 0;
        bufferInfo.range = mUniformBuffers[0]->getSize();

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = mTexture->imageView();
        imageInfo.sampler = mTexture->sampler();

        VkDescriptorBufferInfo lightBufferInfo{};
        lightBufferInfo.buffer = mUniformBuffers[1]->uniformBuffers()[i];
        lightBufferInfo.offset = 0;
        lightBufferInfo.range = mUniformBuffers[1]->getSize();

        VkDescriptorImageInfo shadowSamplerInfo{};
        shadowSamplerInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        shadowSamplerInfo.imageView = mDepthResources->shadowImageView();
        shadowSamplerInfo.sampler = mDepthResources->sampler();

        VkDescriptorBufferInfo directLightBufferInfo{};
        directLightBufferInfo.buffer = mUniformBuffers[2]->uniformBuffers()[i];
        directLightBufferInfo.offset = 0;
        directLightBufferInfo.range = mUniformBuffers[2]->getSize();

        std::array<VkWriteDescriptorSet, 5> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = mDescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;
        descriptorWrites[0].pImageInfo = nullptr; // Optional
        descriptorWrites[0].pTexelBufferView = nullptr; // Optional

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = mDescriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = mDescriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo = &lightBufferInfo;
        descriptorWrites[2].pImageInfo = nullptr; // Optional
        descriptorWrites[2].pTexelBufferView = nullptr; // Optional

        descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[3].dstSet = mDescriptorSets[i];
        descriptorWrites[3].dstBinding = 3;
        descriptorWrites[3].dstArrayElement = 0;
        descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[3].descriptorCount = 1;
        descriptorWrites[3].pImageInfo = &shadowSamplerInfo;

        descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[4].dstSet = mDescriptorSets[i];
        descriptorWrites[4].dstBinding = 4;
        descriptorWrites[4].dstArrayElement = 0;
        descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[4].descriptorCount = 1;
        descriptorWrites[4].pBufferInfo = &directLightBufferInfo;
        descriptorWrites[4].pImageInfo = nullptr; // Optional
        descriptorWrites[4].pTexelBufferView = nullptr; // Optional

        vkUpdateDescriptorSets(mContext->device(), static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}