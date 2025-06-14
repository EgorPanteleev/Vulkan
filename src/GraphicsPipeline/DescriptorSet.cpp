//
// Created by auser on 4/7/25.
//

#include "DescriptorSet.h"
#include "MessageLogger.h"

DescriptorSet::DescriptorSet(Context* context, Texture* texture, VulkanModelLoader* loader,
                             DepthResources* depthResources, const UniformBuffers& uniformBuffers):
                             mContext(context), mTexture(texture), mLoader(loader), mDepthResources(depthResources), mUniformBuffers(uniformBuffers) {
    maxTextures = mLoader->vulkanTextures().size() * ModelTexture::UNKNOWN;
    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();
    updateDescriptorSets();
}

DescriptorSet::~DescriptorSet() {
    vkDestroyDescriptorPool(mContext->device(), mDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mContext->device(), mDescriptorSetLayout, nullptr);
}

void DescriptorSet::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding MVPBinding{
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr
    };

    VkDescriptorSetLayoutBinding shadowSamplerBinding{
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr
    };

    VkDescriptorSetLayoutBinding directLightBinding{
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr
    };

    VkDescriptorSetLayoutBinding pointLightBinding{
            .binding = 3,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr
    };

    VkDescriptorSetLayoutBinding texturesBinding{
            .binding = 4,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = maxTextures,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    std::vector<VkDescriptorSetLayoutBinding> bindings = {MVPBinding          ,
                                                          shadowSamplerBinding,
                                                          directLightBinding  ,
                                                          pointLightBinding   ,
                                                          texturesBinding     };

    std::vector<VkDescriptorBindingFlags> bindingFlags = {
            0,                                                   // binding 0
            0,                                                   // binding 1
            0,                                                   // binding 2
            0,                                                   // binding 3
            VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
            VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT  // binding 4
    };

    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .bindingCount = (uint32_t) bindingFlags.size(),
            .pBindingFlags = bindingFlags.data()
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = &bindingFlagsInfo,
            .bindingCount = (uint32_t) bindings.size(),
            .pBindings = bindings.data()
    };

    if (vkCreateDescriptorSetLayout(mContext->device(), &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
    INFO << "Created descriptor set layout!";
}

void DescriptorSet::createDescriptorPool() {
    auto descriptorCount = (uint32_t)mContext->maxFramesInFlight();
    std::vector<VkDescriptorPoolSize> poolSizes{
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER        , descriptorCount              },
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER        , descriptorCount              },
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount              },
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER        , descriptorCount              },
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxTextures * descriptorCount              }
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

void DescriptorSet::createDescriptorSets() {
    auto descriptorSetCount = (uint32_t)mContext->maxFramesInFlight();
    std::vector<VkDescriptorSetLayout> layouts(descriptorSetCount, mDescriptorSetLayout);
    std::vector<uint32_t> variableCounts(descriptorSetCount, maxTextures);

    VkDescriptorSetVariableDescriptorCountAllocateInfo countAllocInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
            .descriptorSetCount = (uint32_t) mContext->maxFramesInFlight(),
            .pDescriptorCounts = variableCounts.data()
    };

    VkDescriptorSetAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = &countAllocInfo,
            .descriptorPool = mDescriptorPool,
            .descriptorSetCount = (uint32_t) mContext->maxFramesInFlight(),
            .pSetLayouts = layouts.data()
    };

    mDescriptorSets.resize(mContext->maxFramesInFlight());
    if (vkAllocateDescriptorSets(mContext->device(), &allocInfo, mDescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }
    INFO << "Created descriptor sets!";
}

void DescriptorSet::updateDescriptorSets() {
    for (size_t i = 0; i < mContext->maxFramesInFlight(); ++i) {
        VkDescriptorBufferInfo MVPBufferInfo{
                .buffer = mUniformBuffers[0]->uniformBuffers()[i],
                .offset = 0,
                .range = mUniformBuffers[0]->getSize()
        };

        VkDescriptorImageInfo shadowMapInfo{
                .sampler = mDepthResources->sampler(),
                .imageView = mDepthResources->shadowImageView(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkDescriptorBufferInfo directLightBufferInfo{
                .buffer = mUniformBuffers[2]->uniformBuffers()[i],
                .offset = 0,
                .range = mUniformBuffers[2]->getSize()
        };

        VkDescriptorBufferInfo pointLightBufferInfo{
                .buffer = mUniformBuffers[1]->uniformBuffers()[i],
                .offset = 0,
                .range = mUniformBuffers[1]->getSize()
        };

        std::vector<VkDescriptorImageInfo> texturesInfo(maxTextures);
        const std::vector<VulkanTextures>& vulkanTextures = mLoader->vulkanTextures();
        for (size_t j = 0; j < maxTextures; ++j) { //FIXME 2d cycle mTextures not always [0]
            texturesInfo[j] = {
                    .sampler = vulkanTextures[j].mTextures[0]->sampler(),
                    .imageView = vulkanTextures[j].mTextures[0]->imageView(),
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };
        }

        std::vector<VkWriteDescriptorSet> descriptorWrites{
                {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = mDescriptorSets[i],
                        .dstBinding = 0,
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .pImageInfo = nullptr,
                        .pBufferInfo = &MVPBufferInfo,
                        .pTexelBufferView = nullptr
                },
                {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = mDescriptorSets[i],
                        .dstBinding = 1,
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .pImageInfo = &shadowMapInfo

                },
                {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = mDescriptorSets[i],
                        .dstBinding = 2,
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
                        .dstBinding = 3,
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .pImageInfo = nullptr,
                        .pBufferInfo = &pointLightBufferInfo,
                        .pTexelBufferView = nullptr
                },
                {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = mDescriptorSets[i],
                        .dstBinding = 4,
                        .dstArrayElement = 0,
                        .descriptorCount = (uint32_t)texturesInfo.size(),
                        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .pImageInfo = texturesInfo.data()
                },
        };

        vkUpdateDescriptorSets(mContext->device(), static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}