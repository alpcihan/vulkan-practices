#pragma once

#include "shared.h"
#include "wrapper/glfw/window.h"
#include "wrapper/vk/instance.h"
#include "wrapper/glfw/window.h"

namespace vk {

class Surface {
public:
    Surface(const Instance& instance, const glfw::Window& window);
    ~Surface();

    const VkSurfaceKHR& get() const { return m_surface; }

private:
    VkSurfaceKHR m_surface;
    const Instance& m_instance;
};

}