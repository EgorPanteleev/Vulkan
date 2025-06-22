//
// Created by auser on 4/2/25.
//

#include "VulkanApp.h"

#define MODEL_PATH PROJECT_PATH"models/Sponza/glTF/Sponza.gltf"
//#define MODEL_PATH PROJECT_PATH"models/Bistro/BistroExterior.fbx"

int main() {
    VulkanApp app(MODEL_PATH);
    try {
        app.run();
    } catch (const std::exception& e) {
        ERROR << e.what();
        return EXIT_FAILURE;
    }
}