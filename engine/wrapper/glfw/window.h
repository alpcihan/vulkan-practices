#pragma once

#include "wrapper/vk/instance.h"

namespace glfw {

typedef void (*WindowFrameBufferResizeCallback)(uint32_t width, uint32_t height, void* callbackData);

class Window {
public:
    Window(const vk::Instance& instance, uint32_t width = 800, uint32_t height = 600);
    ~Window();

    GLFWwindow* get() const { return m_glfwWindow; }
    const VkSurfaceKHR& getSurface() const { return m_surface; }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

    void getFramebufferSize(int& width, int& height) const;
    void setFramebufferResizeCallback(WindowFrameBufferResizeCallback callback, void* callbackData);

private:
    GLFWwindow* m_glfwWindow;
    VkSurfaceKHR m_surface;
    const vk::Instance& m_instance;

    uint32_t m_width;
    uint32_t m_height;

    WindowFrameBufferResizeCallback m_framebufferResizeCallback = nullptr; // TODO: implement a better callback logic
    void* m_framebufferResizeCallbackData = nullptr; // TODO: implement a better callback logic

    static uint32_t m_windowCount;

private:
    static void _framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
};

} // namespace glfw