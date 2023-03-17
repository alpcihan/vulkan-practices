#include "wrapper/glfw/window.h"

namespace glfw {

Window::Window(const vk::Instance& instance, uint32_t width, uint32_t height)
    : m_instance(instance), m_width(width), m_height(height) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_glfwWindow = glfwCreateWindow(m_width, m_height, "Vulkan", nullptr, nullptr);

    glfwSetWindowUserPointer(m_glfwWindow, this);
    glfwSetFramebufferSizeCallback(m_glfwWindow, _framebufferResizeCallback);

    if (glfwCreateWindowSurface(instance.get(), m_glfwWindow, nullptr, &m_surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

Window::~Window() {
    vkDestroySurfaceKHR(m_instance.get(), m_surface, nullptr);
    glfwDestroyWindow(m_glfwWindow);
}

void Window::getFramebufferSize(int& width, int& height) const {
    glfwGetFramebufferSize(m_glfwWindow, &width, &height);
}

void Window::setFramebufferResizeCallback(WindowFrameBufferResizeCallback callback, void* callbackData) {
    m_framebufferResizeCallbackData = callbackData;
    m_framebufferResizeCallback = callback;
}


uint32_t Window::m_windowCount = 0;

void Window::_framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

    window->m_width = width;
    window->m_height = height;

    if (!window->m_framebufferResizeCallback) {
        return;
    }

    window->m_framebufferResizeCallback(width, height, window->m_framebufferResizeCallbackData);
}

}  // namespace glfw