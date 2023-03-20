#pragma once

#include <vulkan/vulkan.h>

namespace vk {

inline VkApplicationInfo applicationInfo() {
    return {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Vulkan Practices",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };
}

inline VkCommandBufferAllocateInfo commandBufferAllocateInfo() {
    return {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
}

inline VkCommandBufferBeginInfo commandBufferBeginInfo() {
    return {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,                  // optional
        .pInheritanceInfo = nullptr  // optional
    };
}

inline VkBufferCreateInfo bufferCreateInfo() {
    return {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
}

inline VkRenderPassBeginInfo renderPassBeginInfo() {
    static VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    return {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderArea{.offset = {0, 0}},
        .clearValueCount = 1,
        .pClearValues = &clearColor
    };
}

inline VkViewport viewport() {
    return {
        .x = 0.0f,
        .y = 0.0f,
        .minDepth = 0.0f,
        .maxDepth = 1.0
    };
}

inline VkRect2D rect2D() {
    return {
        .offset = {0, 0}
    };
}

}