//
// Created by auser on 6/19/25.
//

#include "VulkanApp.h"

#include <backends/imgui_impl_glfw.h>

bool rightMouseButtonPressed = false;
double lastX = 0.0f, lastY = 0.0f;

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
//    ImGui_ImplGlfw_ScrollCallback(window, xoffset, xoffset);
//    if (ImGui::GetIO().WantCaptureMouse) return;

    auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    Camera* camera = renderer->camera();
    //camera->zoomBy(static_cast<float>(yoffset * 2));
}

static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
//    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
//    if (ImGui::GetIO().WantCaptureKeyboard) return;

    auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS && key == GLFW_KEY_D && mods == GLFW_MOD_CONTROL) {
        renderer->setImGuiUsage(!renderer->imGuiUsage());
    }
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
//    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
//    if (ImGui::GetIO().WantCaptureMouse) return;

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightMouseButtonPressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);  // Initialize lastX and lastY
        } else if (action == GLFW_RELEASE) {
            rightMouseButtonPressed = false;
        }
    }
}

static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
//    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
//    if (ImGui::GetIO().WantCaptureMouse) return;

    auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    Camera* camera = renderer->camera();

    if (!rightMouseButtonPressed || !camera) return;

    double sensitivity = 0.1f;
    double offsetX = xpos - lastX;
    double offsetY = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
    camera->rotate((float)(offsetY * sensitivity), (float)(-offsetX * sensitivity), 0.f);
}

static void frameBufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(glfwWindow));
    Window* window = renderer->window();
    window->setResized(true);
}

static void processKeyboard(GLFWwindow* window, Camera* camera, double deltaTime) {
    auto speed = (float) deltaTime;
    if (speed < 0) return;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->move(speed, 0, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->move(-speed, 0, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->move(0, -speed, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->move(0, speed, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->move(0, 0, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera->move(0, 0, -speed);
    }
}

VulkanApp::VulkanApp(): mRenderer() {
}

void VulkanApp::run() {
    setCallBacks();
    mRenderer.run();
}

void VulkanApp::setCallBacks() {
    auto glfwWindow = mRenderer.context()->window().window();
    glfwSetWindowUserPointer(glfwWindow, &mRenderer);
    glfwSetScrollCallback(glfwWindow, scrollCallback);
    glfwSetCursorPosCallback(glfwWindow, mouseMoveCallback);
    glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
    glfwSetFramebufferSizeCallback(glfwWindow, frameBufferResizeCallback);
    glfwSetKeyCallback(glfwWindow, keyCallBack);
    mRenderer.setKeyboardCallBack(processKeyboard);
}
