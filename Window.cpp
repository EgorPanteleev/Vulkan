//
// Created by auser on 3/21/25.
//

#include "Window.h"

Window::Window( int w, int h, const std::string& name ) {
    initWindow( w, h, name );
}

Window::~Window() {
    glfwDestroyWindow( window );
    glfwTerminate();
}

void Window::initWindow( int w, int h, const std::string& name) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow( w, h, name.c_str(), nullptr, nullptr);
}