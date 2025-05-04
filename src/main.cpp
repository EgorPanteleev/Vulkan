//
// Created by auser on 4/2/25.
//

#include "VulkanApp.h"
#include "MessageLogger.h"

int main() {
    VulkanApp app;
    try {
        app.run();
    } catch (const std::exception& e) {
        ERROR << e.what();
        return EXIT_FAILURE;
    }
}