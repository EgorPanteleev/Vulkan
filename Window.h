//
// Created by auser on 3/21/25.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class Window {
public:
    Window( int w, int h, const std::string& name );
    ~Window();

    [[nodiscard]] bool shouldClose() const {  return glfwWindowShouldClose( window ); }

private:
    void initWindow( int w, int h, const std::string& name );

    GLFWwindow* window;
};



#endif //WINDOW_H
