//
// Created by auser on 4/2/25.
//

#include "Context.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "CommandManager.h"
int main() {
    Context context;
    SwapChain swapChain( &context );
    GraphicsPipeline graphicsPipeline(&context, &swapChain,
                                      "/home/auser/dev/src/Vulkan/CompiledShaders/shader.vert.spv",
                                      "/home/auser/dev/src/Vulkan/CompiledShaders/shader.frag.spv");
    CommandManager commandManager(&context);
}