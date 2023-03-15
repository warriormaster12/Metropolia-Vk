#include "window.h"

#define GLFW_INCLUDE_VULKAN
#define VK_NO_PROTOTYPES
#include "GLFW/glfw3.h"
#include "logger.h"


void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
    MetroWindow::ResizeFramebuffer(true);
}

void MetroWindow::Init(const int& width, const int& height, const std::string& name /*= "Hello window"*/) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* glfw_p_window=nullptr;
    glfw_p_window = glfwCreateWindow(width,height, name.c_str(), nullptr, nullptr);

    p_window = static_cast<void*>(glfw_p_window);

    glfwSetFramebufferSizeCallback(static_cast<GLFWwindow*>(p_window), FramebufferResizeCallback);

    ENGINE_INFO("Window created");
}

bool MetroWindow::Running() {
    if (!glfwWindowShouldClose(static_cast<GLFWwindow*>(p_window))) {
        glfwPollEvents();
        return true;
    }
    return false;
}

void MetroWindow::Shutdown() {
    ENGINE_INFO("Window closed");
    glfwDestroyWindow(static_cast<GLFWwindow*>(p_window));
    glfwTerminate();
}
