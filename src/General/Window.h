//
// Created by auser on 3/21/25.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct WindowCreateInfo {
    const char* name = "";
    int width = 960;
    int height = 600;
};

class Window {
public:
    Window(const WindowCreateInfo& createInfo);
    ~Window();

    void createWindowSurface(VkInstance instance, VkSurfaceKHR& surface);

    void getFrameBufferSize(int& width, int& height);

    [[nodiscard]] bool shouldClose() const {  return glfwWindowShouldClose( mWindow ); }

    void setResized(bool resized) { mFrameBufferResized = resized; }
    bool frameBufferResized() const { return mFrameBufferResized; }

    GLFWwindow* window() { return mWindow; }
private:
    void initWindow(const WindowCreateInfo& createInfo);

    GLFWwindow* mWindow;
    bool mFrameBufferResized;
};



#endif //WINDOW_H
