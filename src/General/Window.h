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

    void createWindowSurface( VkInstance instance, VkSurfaceKHR& surface );

    void getFrameBufferSize( int& width, int& height );

    [[nodiscard]] bool shouldClose() const {  return glfwWindowShouldClose( mWindow ); }

    void setResized(bool resized) { mFrameBufferResized = resized; }
    bool frameBufferResized() { return mFrameBufferResized; }

    GLFWwindow* window() { return mWindow; }
private:
    void initWindow( int w, int h, const std::string& name );

    GLFWwindow* mWindow;
    bool mFrameBufferResized;
};



#endif //WINDOW_H
