//
// Created by igor on 8/23/25.
//

#include "TracyContext.hpp"
#include "Context.hpp"

TracyContext::TracyContext(): mContext(nullptr) {}

TracyContext::TracyContext(Context* context) {
    init(context);
}

TracyContext::~TracyContext() {
    destroy();
}

void TracyContext::init(Context* context) {
    mContext = context;
    mCommandPool = Utils::createCommandPool(mContext, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    mCommandBuffer = Utils::createCommandBuffer(mContext->device(),mCommandPool);

    mTracyContext = TracyVkContext(mContext->physicalDevice(), mContext->device(),
                                   mContext->graphicsQueue(), mCommandBuffer);
}

void TracyContext::destroy() {
    if (!mContext) return;
    TracyVkDestroy(mTracyContext);
    vkFreeCommandBuffers( mContext->device(), mCommandPool,
                          1, &mCommandBuffer );
    vkDestroyCommandPool(mContext->device(), mCommandPool, nullptr);
    mContext = nullptr;
}
