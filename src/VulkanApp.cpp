//
// Created by auser on 5/4/25.
//

#include "VulkanApp.h"

bool rightMouseButtonPressed = false;
double lastX = 0.0f, lastY = 0.0f;

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
    //camera->zoomBy(static_cast<float>(yoffset * 2));
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightMouseButtonPressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);  // Initialize lastX and lastY
        } else if (action == GLFW_RELEASE) {
            rightMouseButtonPressed = false;
        }
    }
}

void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    auto camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    if (!rightMouseButtonPressed || !camera) return;

    double sensitivity = 0.1f;
    double offsetX = xpos - lastX;
    double offsetY = lastY - ypos;  // Inverted since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera->setYaw(camera->yaw() - offsetX * sensitivity);
    camera->setPitch(glm::clamp(camera->pitch() + offsetY * sensitivity, -89.0, 89.0));
}

VulkanApp::VulkanApp(): renderer() {
    auto glfwWindow = renderer.context()->window().window();
    glfwSetWindowUserPointer(glfwWindow, renderer.camera());
    glfwSetScrollCallback(glfwWindow, scrollCallback);
    glfwSetCursorPosCallback(glfwWindow, mouseMoveCallback);
    glfwSetMouseButtonCallback(glfwWindow, MouseButtonCallback);
}

void VulkanApp::run() {
    renderer.run();
}