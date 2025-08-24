//
// Created by igor on 8/23/25.
//

#ifndef VULKAN_TRACYCONTEXT_HPP
#define VULKAN_TRACYCONTEXT_HPP

#include <vulkan/vulkan.h>
#include <tracy/TracyVulkan.hpp>

class Context;

class TracyContext {
public:
    TracyContext();
    TracyContext(Context* context);
    ~TracyContext();

    void init(Context* context);
    void destroy();

    TracyVkCtx get() const { return mTracyContext; }

    TracyVkCtx operator()() const { return get(); }
protected:
    Context* mContext;
    TracyVkCtx mTracyContext;
    VkCommandPool mCommandPool;
    VkCommandBuffer mCommandBuffer;
};

#endif //VULKAN_TRACYCONTEXT_HPP
