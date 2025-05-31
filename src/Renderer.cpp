//
// Created by auser on 4/4/25.
//

#include "Renderer.h"
#include "Clock.h"
#include "MessageLogger.h"
#include "ModelUniformBuffer.h"
#include "LightUniformBuffer.h"
#include "DirectionalLightBuffer.h"
#include "Utils.h"
#include "Image.h"

#define MODEL_PATH PROJECT_PATH"models/Sponza/glTF/Sponza.gltf"

#define TEXTURE_PATH PROJECT_PATH"textures/statue.jpg"

#define COMPILED_SHADERS_PATH BINARY_PATH"compiled_shaders/"

//static std::string MODEL_PATH =
//        "/home/auser/dev/src/Vulkan/models/lamborghini/source/2022_Lamborghini_Countach_LPI_800-4_LBWK_3D_Assetto/ksp_lambo_countach_lbwk/lambo_countach_2022.obj";
//static std::string TEXTURE_PATH =
//        "/home/auser/dev/src/Vulkan/models/lamborghini/textures/Countach50_01_InteriorA_.png";

Renderer::Renderer() {
    //TODO make all raw pointers unique
    mCamera = std::make_unique<Camera>(45.0f, 1280.0f / 720.0f, 0.1f, 1000000);
    mContext = std::make_unique<Context>();
    mTexture = std::make_unique<Image>(mContext.get(), TEXTURE_PATH);
    mSwapChain = std::make_unique<SwapChain>(mContext.get());
    mColorResources = std::make_unique<ColorResources>(mContext.get(), mSwapChain->extent(), mSwapChain->format());
    mDepthResources = std::make_unique<DepthResources>(mContext.get(), mSwapChain->extent());
    mUniformBuffers = std::make_unique<UniformBuffers>();
    mUniformBuffers->emplace_back(std::make_unique<ModelUniformBuffer>(mContext.get(), mCamera.get()));
    mUniformBuffers->emplace_back(std::make_unique<LightUniformBuffer>(mContext.get(), mCamera.get()));
    mUniformBuffers->emplace_back(std::make_unique<DirectionalLightBuffer>(mContext.get(), mCamera.get()));

    mShadowDescriptorSet = std::make_unique<ShadowDescriptorSet>(mContext.get(), mDepthResources.get(), *mUniformBuffers);
    loadShader(COMPILED_SHADERS_PATH"shadowShader.vert.spv", mShadowVertShaderModule);
    mShadowPipeline = std::make_unique<ShadowPipeline>(mContext.get(), mShadowDescriptorSet.get(), mShadowVertShaderModule);

    mDescriptorSet = std::make_unique<DescriptorSet>(mContext.get(), mTexture.get(), *mUniformBuffers);
    loadShader(COMPILED_SHADERS_PATH"shader.vert.spv", mMainVertShaderModule);
    loadShader(COMPILED_SHADERS_PATH"shader.frag.spv", mFragShaderModule);
    mGraphicsPipeline = std::make_unique<GraphicsPipeline>(mContext.get(), mSwapChain.get(), mDescriptorSet.get(),
                                                           mMainVertShaderModule, mFragShaderModule);
    mCommandManager = std::make_unique<CommandManager>(mContext.get());
    mVertexBuffer = std::make_unique<VertexBuffer>(mContext.get(), MODEL_PATH);
    mSyncObjects = std::make_unique<SyncObjects>(mContext.get(), mSwapChain.get());


    mSwapChain->createFrameBuffers(mGraphicsPipeline->renderPass(),
                                   mDepthResources->imageView(),
                                   mColorResources->imageView());
    mSwapChain->createShadowFrameBuffers(mShadowPipeline->renderPass(), mDepthResources->shadowImageView());
}

void processKeyboard(Camera& camera, GLFWwindow* window) {
    float speed = 2.5f * 1;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.setPosition(camera.position() + camera.forward() * speed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.setPosition(camera.position() - camera.forward() * speed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.setPosition(camera.position() - glm::normalize(glm::cross(camera.forward(), camera.up())) * speed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.setPosition(camera.position() + glm::normalize(glm::cross(camera.forward(), camera.up())) * speed);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.setPosition( camera.position() + camera.up() * speed );
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.setPosition( camera.position() - camera.up() * speed );
    }

}

Renderer::~Renderer() {
    vkDestroyShaderModule( mContext->device(), mShadowVertShaderModule, nullptr );
    vkDestroyShaderModule( mContext->device(), mMainVertShaderModule, nullptr );
    vkDestroyShaderModule( mContext->device(), mFragShaderModule, nullptr );
}

void Renderer::run() {
    mainLoop();
}

void Renderer::mainLoop() {
    bool fpsCounted = true;
    Clock clock;
    int frames = 0;
    while ( !mContext->window().shouldClose() ) {
        glfwPollEvents();
        if (fpsCounted) {
            clock.start();
            fpsCounted = false;
        }
        drawFrame();
        processKeyboard( *mCamera, mContext->window().window() );
        ++frames;
        clock.stop();
        if (clock.duration() > 0.1) {
            std::string title = "FPS: " + std::to_string(frames / clock.duration());
            glfwSetWindowTitle(mContext->window().window(), title.c_str());
            fpsCounted = true;
            frames = 0;
        }
    }
    vkDeviceWaitIdle( mContext->device() );
}

void Renderer::drawFrame() {
    uint32_t imageIndex;
    auto acquireResult = mSwapChain->acquireNextImage(&imageIndex,
                                 mSyncObjects->imageAvailableSemaphore(mSwapChain->currentFrame()),
                                 mSyncObjects->inFlightFence(mSwapChain->currentFrame()));

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR ) {
        recreateSwapChain();
        return;
    } else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    // Only reset the fence if we are submitting work
    vkResetFences(mContext->device(), 1, &mSyncObjects->inFlightFence(mSwapChain->currentFrame()));

    for ( auto& uniformBuffer: *mUniformBuffers ) {
        uniformBuffer->updateUniformBuffer(mSwapChain->currentFrame(), mSwapChain->extent() );
    }

    mCommandManager->recordCommandBuffer( mSwapChain.get(), mGraphicsPipeline.get(), mShadowPipeline.get(),
                                         mDescriptorSet.get(), mShadowDescriptorSet.get(), mVertexBuffer.get(), imageIndex );
    auto submitResult = mCommandManager->submitCommandBuffer( mSwapChain.get(), mSyncObjects.get(), &imageIndex );
    
    if (submitResult == VK_ERROR_OUT_OF_DATE_KHR ||
            submitResult == VK_SUBOPTIMAL_KHR || mContext->window().frameBufferResized()) {
        mContext->window().setResized(false);
        recreateSwapChain();
        return;
    } else if (submitResult != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit command buffer!");
    }
    mSwapChain->updateCurrentFrame();
}

void Renderer::recreateSwapChain() {
    vkDeviceWaitIdle(mContext->device());
    mSwapChain->clear();
    mSwapChain->recreate();
    mColorResources->recreate(mSwapChain->extent(), mSwapChain->format());
    mDepthResources->recreate(mSwapChain->extent());
    mSwapChain->createFrameBuffers(mGraphicsPipeline->renderPass(),
                                   mDepthResources->imageView(), mColorResources->imageView());
}

void Renderer::loadShader(const std::string& shaderPath, VkShaderModule& module) {
    auto shaderCode = Utils::readFile(shaderPath);
    Utils::createShaderModule(mContext->device(), shaderCode, &module);
}
