//
// Created by auser on 3/21/25.
//

#include "Window.h"
#include "MessageLogger.h"

#include <stdexcept>
#include <iostream>

static void frameBufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    window->setResized(true);
}

Window::Window( int w, int h, const std::string& name ): mFrameBufferResized(false) {
    initWindow( w, h, name );
}

Window::~Window() {
    glfwDestroyWindow( mWindow );
    glfwTerminate();
}

void Window::initWindow( int w, int h, const std::string& name) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    mWindow = glfwCreateWindow( w, h, name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, frameBufferResizeCallback);
}

void Window::createWindowSurface( VkInstance instance, VkSurfaceKHR& surface ) {
    if (glfwCreateWindowSurface(instance, mWindow, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
    INFO << "Window surface created!";
}

void Window::getFrameBufferSize( int& width, int& height ) {
    glfwGetFramebufferSize(mWindow, &width, &height);
}
