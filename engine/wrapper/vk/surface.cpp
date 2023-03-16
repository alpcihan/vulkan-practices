#include "wrapper/vk/surface.h"

namespace vk {

Surface::Surface(const Instance& instance, const glfw::Window& window)
    : m_instance(instance) {
    if (glfwCreateWindowSurface(m_instance.get(), window.get(), nullptr, &m_surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

}

Surface::~Surface() {
    vkDestroySurfaceKHR(m_instance.get(), m_surface, nullptr);
}

}