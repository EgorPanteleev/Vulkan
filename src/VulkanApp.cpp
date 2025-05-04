//
// Created by auser on 5/4/25.
//

#include "VulkanApp.h"

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
    camera->zoomBy(static_cast<float>(yoffset * 2));
}

void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = -1;
    static double lastY = -1;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (lastX >= 0 && lastY >= 0) {
            auto dx = static_cast<float>(lastX - xpos);
            auto dy = static_cast<float>(ypos - lastY);

            auto camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
            camera->rotate(dx * 0.2f, dy * 0.2f);
        }

        lastX = xpos;
        lastY = ypos;
    } else {
        lastX = -1;
        lastY = -1;
    }
}

VulkanApp::VulkanApp(): renderer() {
    auto glfwWindow = renderer.context()->window().window();
    glfwSetWindowUserPointer(glfwWindow, renderer.camera());
    glfwSetScrollCallback(glfwWindow, scrollCallback);
    glfwSetCursorPosCallback(glfwWindow, mouseMoveCallback);
}

void VulkanApp::run() {
    renderer.run();
}