//
// Created by auser on 4/2/25.
//

#include "VulkanApp.hpp"
#include "tracy/Tracy.hpp"

#define MODEL_PATH PROJECT_PATH"models/Sponza/glTF/Sponza.gltf"
//#define MODEL_PATH PROJECT_PATH"models/Bistro/BistroExterior.fbx"

int main() {
    CameraCreateInfo cameraCreateInfo{
        .pos = glm::vec3(0),
        .target = glm::vec3(-1, 0, 0),
        .up = glm::vec3(0, 1, 0),
        .FOV = 45.0f,
        .aspectRatio = 1920.0f / 1200.0f,
        .nearPlane = 0.1f,
        .farPlane = 10000.f
    };

    VulkanApp app(MODEL_PATH, cameraCreateInfo);
    try {
        app.run();
    } catch (const std::exception& e) {
        ERROR << e.what();
        return EXIT_FAILURE;
    }
}